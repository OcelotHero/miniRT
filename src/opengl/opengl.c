/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   opengl.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 05:26:47 by rraharja          #+#    #+#             */
/*   Updated: 2023/09/26 12:50:44 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define BONUS
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_resize.h"
#include "MLX42/MLX42_Int.h"

#include "types.h"

#include <errno.h>

/**
 * Creates, compiles, and returns a shader object of the given type from the
 * given shader source code.
 *
 * @param	type			Shader type to compile
 * @param	shader_source	Shader source code to compile
 * @return	Reference ID to shader object or 0 if an error occurs
 */
static uint32_t	create_shader(int32_t type, char *shader_source)
{
	int			success;
	char		log[512];
	uint32_t	shader;

	if (!shader_source)
		return (0);
	shader = glCreateShader(type);
	if (!shader)
		return (0);
	glShaderSource(shader, 1, (const GLchar *const *)&shader_source, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, log);
		if (type == GL_VERTEX_SHADER)
			printf("ERROR::VERTEX SHADER::COMPILATION_FAILED\n%s\n", log);
		else
			printf("ERROR::FRAGMENT SHADER::COMPILATION_FAILED\n%s\n", log);
	}
	free(shader_source);
	return ((success != 0) * shader);
}

/**
 * Opens the shader source code file and returns it as string.
 *
 * @param	source	Path to shader source code
 * @return	Source code string
 */
char	*shader_source(const char *source)
{
	char	*buffer;
	long	length;
	FILE	*f;

	f = fopen(source, "r");
	if (!f && dprintf(2, "fopen: %s\n", strerror(errno)))
		return (NULL);
	fseek(f, 0, SEEK_END);
	length = ftell(f);
	fseek(f, 0, SEEK_SET);
	buffer = malloc(length + 1);
	if (buffer && fread(buffer, 1, length, f) >= 0)
		buffer[length] = '\0';
	if (ferror(f) && (fclose(f) || 1))
	{
		free(buffer);
		return (NULL);
	}
	fclose(f);
	if (!buffer && dprintf(2, "malloc: %s\n", strerror(errno)))
		return (NULL);
	return (buffer);
}

/**
 * Creates and links a program object using the given file path to the vertex
 * and fragment shader source code.
 *
 * @param	vert	Path to vertex shader source code
 * @param	frag	Path to fragment shader source code
 * @return	Reference ID to program object or 0 if an error occurs
 */
uint32_t	create_shader_program(const char *vert, const char *frag)
{
	int			success;
	char		log[512];
	uint32_t	program;
	uint32_t	vshader;
	uint32_t	fshader;

	success = 0;
	program = glCreateProgram();
	vshader = create_shader(GL_VERTEX_SHADER, shader_source(vert));
	fshader = create_shader(GL_FRAGMENT_SHADER, shader_source(frag));
	if (program && vshader && fshader)
	{
		glAttachShader(program, vshader);
		glAttachShader(program, fshader);
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(program, 512, NULL, log);
			printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", log);
		}
	}
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	return ((success != 0 && vshader && fshader) * program);
}

#define WIDTH	1600
#define HEIGHT	900

# ifdef __APPLE__
#  define DENSITY 2.0f
# else
#  define DENSITY 1.0f
# endif

#define SP	0x00800
#define	CY	0x02000
#define	CN	0x04000
#define	BX	0x08000
#define PL	0x01000
#define	QD	0x10000
#define	DS	0x20000
#define	TR	0x40000

float		deltaTime = 0.0f;
float		lastFrame = 0.0f;
bool		resetFrame = false;
t_scene		scene;

t_vec4	vec4_elem_op(t_vec4 a, char op, t_vec4 b);
t_vec4	vec4_scale(float a, t_vec4 v);
t_vec4	vec4_normalize(t_vec4 a);
float	vec4_dot(t_vec4 a, t_vec4 b);
float	vec4_length(t_vec4 a);

t_vec3	vec3_normalize(t_vec3 a);

void	set_scene_geometry(t_scene *scene)
{
	int	n = 0;

	// back wall
	scene->objects[n++] = (t_object){.type = QD,
		.pos = {   0.00f,   0.00f,   5.00f,   0.00f},
			   {  12.60f,   0.00f,   5.00f,   0.00f},
			   {   0.00f,  12.60f,   5.00f,   0.00f}};
	// floor
	scene->objects[n++] = (t_object){.type = QD,
		.pos = {   0.00f, -12.45f,   0.00f,   0.00f},
			   {  12.60f, -12.45f,   0.00f,   0.00f},
			   {   0.00f, -12.45f,   5.00f,   0.00f}};
	// ceiling
	scene->objects[n++] = (t_object){.type = QD,
		.pos = {   0.00f,  12.50f,   0.00f,   0.00f},
			   {   0.00f,  12.50f,   5.00f,   0.00f},
			   {  12.60f,  12.50f,   0.00f,   0.00f}};
	// left wall
	scene->objects[n++] = (t_object){.type = QD,
		.pos = { -12.50f,   0.00f,   0.00f,   0.00f},
			   { -12.50f,  12.60f,   0.00f,   0.00f},
			   { -12.50f,   0.00f,   5.00f,   0.00f}};
	// right wall
	scene->objects[n++] = (t_object){.type = QD,
		.pos = {  12.50f,   0.00f,   0.00f,   0.00f},
			   {  12.50f,  12.60f,   0.00f,   0.00f},
			   {  12.50f,   0.00f,   5.00f,   0.00f}};
	// light
	scene->objects[n++] = (t_object){.type = QD,
		.pos = {   0.00f,  12.40f,   0.00f,   0.00f},
			   {   5.00f,  12.40f,   0.00f,   0.00f},
			   {   0.00f,  12.40f,   2.50f,   0.00f}};

	// picture
	scene->objects[n++] = (t_object){.type = QD,
		.pos = {   0.00f,   0.00f,   4.90f,   0.00f},
			   {   0.00f,   3.50f,   4.90f,   0.00f},
			   {   5.50f,   0.00f,   4.90f,   0.00f}};
	// strip pattern
	scene->objects[n++] = (t_object){.type = QD,
		.pos = {   0.00f,  -8.50f,   4.90f,   0.00f},
			   {   0.00f,  -5.00f,   4.90f,   0.00f},
			   {   5.50f,  -8.50f,   4.90f,   0.00f}};

	// spheres of varying specular roughness
	scene->objects[n++] = (t_object){.type = SP,
		.pos		= { -10.00f,   0.00f,   3.00f,   0.00f},
		.param[0]	= 1.75f};
	scene->objects[n++] = (t_object){.type = SP,
		.pos		= {  -5.00f,   0.00f,   3.00f,   0.00f},
		.param[0]	= 1.75f};
	scene->objects[n++] = (t_object){.type = SP,
		.pos		= {   0.00f,   0.00f,   3.00f,   0.00f},
		.param[0]	= 1.75f};
	scene->objects[n++] = (t_object){.type = SP,
		.pos		= {   5.00f,   0.00f,   3.00f,   0.00f},
		.param[0]	= 1.75f};
	scene->objects[n++] = (t_object){.type = SP,
		.pos		= {  10.00f,   0.00f,   3.00f,   0.00f},
		.param[0]	= 1.75f};

	scene->objects[n++] = (t_object){.type = CY,
		.pos		= {  -7.00f,  -9.90f,   0.00f,   0.00f},
		.axis		= {   0.00f,   0.90f,   0.00f,   0.00f},
		.param		= {2.f, 2.5f, 0.f, 0.f}};
	scene->objects[n++] = (t_object){.type = CY,
		.pos		= {  -7.00f, -10.00f,   1.00f,   0.00f},
		.axis		= {   0.00f,   0.90f,   0.00f,   0.00f},
		.param		= {0.5f, 2.f, 0.f, 0.f}};
	scene->objects[n++] = (t_object){.type = CY,
		.pos		= {  -7.00f, -10.00f,  -1.00f,   0.00f},
		.axis		= {   0.00f,   0.90f,   0.00f,   0.00f},
		.param		= {0.5f, 2.f, 0.f, 0.f}};


	scene->objects[n++] = (t_object){.type = CY,
		.pos		= {   0.00f,  -8.00f,   0.00f,   0.00f},
		.axis		= {   0.00f,   0.00f,   0.90f,   0.00f},
		.param		= {2.f, 2.5f, 0.f, 0.f}};

	scene->n_obj = n;
}

void	set_scene_material(t_scene *scene)
{
	int	n = 0;

	// back wall
	scene->materials[n++] = (t_material){.albedo = {   0.7f,   0.7f,   0.7f,   0.0f},
										 .IOR = 1.0f};
	// floor
	scene->materials[n++] = (t_material){.albedo = {  64.0f, 128.0f,   0.0f,  -1.0f},
										 .IOR = 1.0f};
	// ceiling
	scene->materials[n++] = (t_material){.albedo = {   0.7f,   0.7f,   0.7f,   0.0f},
										 .IOR = 1.0f};
	// left wall
	scene->materials[n++] = (t_material){.albedo = {   0.7f,   0.1f,   0.1f,   0.0f},
										 .IOR = 1.0f};
	// right wall
	scene->materials[n++] = (t_material){.albedo = {   0.1f,   0.7f,   0.1f,   0.0f},
										 .IOR = 1.0f};
	// light
	scene->materials[n++] = (t_material){.emissive = {   1.0f,   0.9f,   0.5f,   0.0f},
										 .intensity = 25.0f,
										 .IOR = 1.0f};

	// picture
	scene->materials[n++] = (t_material){.albedo = {   0.0f,   0.0f,   0.0f,   3.0f},
										 .IOR = 1.0f};
	// strip pattern
	scene->materials[n++] = (t_material){.albedo = {  64.0f,   0.0f,   0.0f,  -1.0f},
										 .IOR = 1.0f};

	// spheres of varying specular roughness
	scene->materials[n++] = (t_material){.albedo			= {   1.0f,   1.0f,   1.0f,   0.0f},
										 .spec_color		= {   0.3f,   1.0f,   0.3f,   0.0f},
										 .spec_chance		= 1.0f,
										 .spec_roughness	= 0.0f,
										 .IOR = 1.0f};
	scene->materials[n++] = (t_material){.albedo			= {   1.0f,   1.0f,   1.0f,   0.0f},
										 .spec_color		= {   0.3f,   1.0f,   0.3f,   0.0f},
										 .spec_chance		= 1.0f,
										 .spec_roughness	= 0.25f,
										 .IOR = 1.0f};
	scene->materials[n++] = (t_material){.albedo			= {   1.0f,   1.0f,   1.0f,   0.0f},
										 .spec_color		= {   0.3f,   1.0f,   0.3f,   0.0f},
										 .spec_chance		= 1.0f,
										 .spec_roughness	= 0.5f,
										 .IOR = 1.0f};
	scene->materials[n++] = (t_material){.albedo			= {   1.0f,   1.0f,   1.0f,   0.0f},
										 .spec_color		= {   0.3f,   1.0f,   0.3f,   0.0f},
										 .spec_chance		= 1.0f,
										 .spec_roughness	= 0.75f,
										 .IOR = 1.0f};
	scene->materials[n++] = (t_material){.albedo			= {   1.0f,   1.0f,   1.0f,   0.0f},
										 .spec_color		= {   0.3f,   1.0f,   0.3f,   0.0f},
										 .spec_chance		= 1.0f,
										 .spec_roughness	= 1.0f,
										 .IOR = 1.0f};


	scene->materials[n++] = (t_material){.albedo			= {   0.9f,   0.9f,   0.9f,   0.0f},
										 .spec_color		= {   0.9f,   0.9f,   0.9f,   0.0f},
										 .refr_color		= {   0.0f,   0.0f,   0.0f,   0.0f},
										 .spec_chance		= 0.02f,
										 .refr_chance		= 0.96f,
										 .IOR = 1.5f};
	scene->materials[n++] = (t_material){.albedo			= {   0.9f,   0.9f,   0.9f,   0.0f},
										 .spec_color		= {   0.9f,   0.9f,   0.9f,   0.0f},
										 .refr_color		= {   0.1f,   0.4f,   1.0f,   0.0f},
										 .spec_chance		= 0.0002f,
										 .refr_chance		= 1.00f,
										 .intensity			= 1.0f,
										 .IOR = 1.5f};
	scene->materials[n++] = (t_material){.albedo			= {   0.9f,   0.9f,   0.9f,   0.0f},
										 .spec_color		= {   0.9f,   0.9f,   0.9f,   0.0f},
										 .refr_color		= {   1.0f,   0.4f,   0.0f,   0.0f},
										 .spec_chance		= 0.0002f,
										 .refr_chance		= 1.00f,
										 .intensity			= 1.0f,
										 .IOR = 1.5f};

	scene->materials[n++] = (t_material){.albedo			= {   0.9f,   0.9f,   0.9f,   0.0f},
										 .spec_color		= {   0.9f,   0.9f,   0.9f,   0.0f},
										 .refr_color		= {   0.0f,   0.0f,   0.0f,   0.0f},
										 .spec_chance		= 0.02f,
										 .refr_chance		= 1.00f,
										 .IOR = 1.5f};
}

void	set_scene_light(t_scene *scene)
{
	int	n = 0;

	scene->ambient = (t_object){.param  = {0.9f, 0.9f, 0.0f, 0.f}};
	scene->lights[n++] = (t_object){.pos	= { -12.40f, -10.00f,   0.00f,   0.00f},
									.param	= { 1.0f, 0.9f, 0.5f, 1.0f}};
	scene->lights[n++] = (t_object){.pos	= { -12.40f,  10.00f,   0.00f,   0.00f},
									.param	= { 1.0f, 0.9f, 0.5f, 1.0f}};

	scene->n_light = n;
}

int	move_scene_to_buffer(t_scene *scene)
{
	int			i;
	t_object	*obj;

	glBufferSubData(GL_UNIFORM_BUFFER, 0, 45136, &scene->n_obj);
	glBufferSubData(GL_UNIFORM_BUFFER, 45312, 4, &scene->n_light);
	glBufferSubData(GL_UNIFORM_BUFFER, 45328, 16, &scene->ambient.param);
	i = -1;
	while (++i < scene->n_light)
	{
		obj = &scene->lights[i];
		glBufferSubData(GL_UNIFORM_BUFFER, 45344 + i * 32, 16, &obj->pos);
		glBufferSubData(GL_UNIFORM_BUFFER, 45360 + i * 32, 16, &obj->param);
	}
}

GLuint	setup_and_load_scene(GLuint	program)
{
	uint32_t	ubo, objects, lights;

	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, 53536, NULL, GL_STATIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 45248);
	glBindBufferRange(GL_UNIFORM_BUFFER, 1, ubo, 45312, 8192);

	lights = glGetUniformBlockIndex(program, "Lights");
	objects = glGetUniformBlockIndex(program, "Objects");
	glUniformBlockBinding(program, lights, 1);
	glUniformBlockBinding(program, objects, 0);

	scene.camera = (t_object){.pos		= {0.f, 0.f, -30.f,  0.f},
							  .axis		= {0.f, 0.f,   1.f,  0.f},
							  .param	= {0.f, 0.f,   0.f, 90.f}};
	set_scene_geometry(&scene);
	set_scene_material(&scene);
	set_scene_light(&scene);
	move_scene_to_buffer(&scene);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	return (ubo);
}

int	load_texture(char *path)
{
	int		width, height, channels;
	GLuint	tex;
	uint8_t	*data;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load(path, &width, &height, &channels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB + (channels == 4), width, height,
			0, GL_RGB + (channels == 4), GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		printf("%s tex failed to load\n", path);
		return (-1);
	}
	stbi_image_free(data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	return (tex);
}

void	processInput(GLFWwindow	*window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	const float	cameraSpeed = 5.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		scene.camera.pos = vec4_elem_op(scene.camera.pos, '+', vec4_scale(cameraSpeed, scene.camera.axis));
		resetFrame = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		scene.camera.pos = vec4_elem_op(scene.camera.pos, '-', vec4_scale(cameraSpeed, scene.camera.axis));
		resetFrame = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		scene.camera.pos = vec4_elem_op(scene.camera.pos, '+',
			vec4_scale(cameraSpeed, vec4_elem_op(scene.camera.axis, 'x', (t_vec4){0.f, 1.f, 0.f, 0.f})));
		resetFrame = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		scene.camera.pos = vec4_elem_op(scene.camera.pos, '-',
			vec4_scale(cameraSpeed, vec4_elem_op(scene.camera.axis, 'x', (t_vec4){0.f, 1.f, 0.f, 0.f})));
		resetFrame = true;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		scene.camera.pos.y += cameraSpeed;
		resetFrame = true;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		scene.camera.pos.y -= cameraSpeed;
		resetFrame = true;
	}
}

double	lastX, lastY;
double	yaw, pitch;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == MLX_PRESS)
		glfwGetCursorPos(window, &lastX, &lastY);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	double	delta[2] = {xpos - lastX, ypos - lastY};

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		float xoffset = (delta[0] > 0) - (delta[0] < 0);
		float yoffset = (delta[1] > 0) - (delta[1] < 0);
		float sensitivity = 0.2f;

		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		scene.camera.axis.x = cos(yaw * M_PI / 180.0) * cos(pitch* M_PI / 180.0);
		scene.camera.axis.y = sin(pitch* M_PI / 180.0);
		scene.camera.axis.z = sin(yaw * M_PI / 180.0) * cos(pitch* M_PI / 180.0);

		resetFrame = true;
	}
	lastX = xpos;
	lastY = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	scene.camera.param[3] -= (float)yoffset;
	if (scene.camera.param[3]  < 1.0f)
		scene.camera.param[3]  = 1.0f;
	if (scene.camera.param[3]  > 179.0f)
		scene.camera.param[3]  = 179.0f;
	resetFrame = true;
}

int main()
{
	int width;
	int height;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenglContext", NULL, NULL);
	if (!window)
	{
		dprintf(2, "failed to create window\n");
		exit(-1);
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		dprintf(2, "failed to initialize glad with processes\n");
		exit(-1);
	}

	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	int samples = 4;
	float quadVerts[] = {
		-1.0, -1.0,
		-1.0, 1.0,
		1.0, -1.0,

		1.0, -1.0,
		-1.0, 1.0,
		1.0, 1.0
	};

	GLuint	vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint	vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint	framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	GLuint	iChannel0;
	glGenTextures(1, &iChannel0);
	glBindTexture(GL_TEXTURE_2D, iChannel0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (int)(width / DENSITY), (int)(height/ DENSITY), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, iChannel0, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLuint	cubemap;
	glGenTextures(1, &cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

	char	*faces[6] = {"res/cubemaps/GalleryFront.jpg",
						 "res/cubemaps/GalleryBack.jpg",
						 "res/cubemaps/GalleryTop.jpg",
						 "res/cubemaps/GalleryBottom.jpg",
						 "res/cubemaps/GalleryLeft.jpg",
						 "res/cubemaps/GalleryRight.jpg"};

	int	im_width, im_height, im_channels;
	uint8_t	*data;

	for (uint32_t i = 0; i < 6; i++) {
		data = stbi_load(faces[i], &im_width, &im_height, &im_channels, 0);
		if (data)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB + (im_channels == 4),
						 im_width, im_height, 0, GL_RGB + (im_channels == 4), GL_UNSIGNED_BYTE, data);
		else
			printf("Cubemap tex failed to load at path: %s\n", faces[i]);
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	uint32_t	buf_a_program = create_shader_program("res/shaders/buffer_a.vert", "res/shaders/buffer_a.frag");
	uint32_t	image_program = create_shader_program("res/shaders/image.vert", "res/shaders/image.frag");
	if (!buf_a_program && !image_program && dprintf(2, "Error\n"))
		exit(-1);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	int	tex[16] = {0};

	tex[2] = load_texture("res/textures/Organic4.jpg");
	tex[3] = load_texture("res/textures/Earthmap.jpg");

	glUseProgram(buf_a_program);
	glUniform1i(glGetUniformLocation(buf_a_program, "skybox"), 0);
	glUniform1i(glGetUniformLocation(buf_a_program, "framebuffer"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, iChannel0);

	for (int i = 2; i < 16; ++i) {
		char	buffer[10];

		sprintf(buffer, "tex%02d", i);
		glUniform1i(glGetUniformLocation(buf_a_program, buffer), i);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, tex[i]);
	}

	glUseProgram(image_program);
	glUniform1i(glGetUniformLocation(image_program, "iChannel0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, iChannel0);


	GLuint	ubo = setup_and_load_scene(buf_a_program);
	t_vec4	v = scene.camera.axis;
	pitch = 90.0 - acos(v.y / sqrt(v.x * v.x + v.y * v.y + v.z * v.z)) * 180.0 / M_PI;
	yaw = ((v.z >= 0) - (v.z < 0)) * acos(v.x / sqrt(v.x * v.x + v.z * v.z)) * 180.0 / M_PI;

	glBindVertexArray(vao);
	float	startFrame = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		float	currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		if (resetFrame) {
			glBindBuffer(GL_UNIFORM_BUFFER, ubo);
			glBufferSubData(GL_UNIFORM_BUFFER, 16, 64, &scene.camera);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			startFrame = glfwGetTime();
			resetFrame = false;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glViewport(0, 0, (int)(width / DENSITY), (int)(height/ DENSITY));
		glUseProgram(buf_a_program);
		glUniform2fv(glGetUniformLocation(buf_a_program, "iResolution"), 1, (float []){WIDTH, HEIGHT});
		glUniform1i(glGetUniformLocation(buf_a_program, "iFrame"), (int)((glfwGetTime() - startFrame) * 60));
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		glUseProgram(image_program);
		glUniform2fv(glGetUniformLocation(image_program, "iResolution"), 1, (float []){WIDTH, HEIGHT});
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}
