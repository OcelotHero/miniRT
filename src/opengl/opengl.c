/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   opengl.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 05:26:47 by rraharja          #+#    #+#             */
/*   Updated: 2023/08/26 23:59:08 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "MLX42/MLX42_Int.h"
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
	char	log[512];
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

int main()
{
	int width;
	int height;

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
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

	int	texture_units;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
	printf("Max accessible texture per stage: %d\n", texture_units);

	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	// GLint no_of_extensions = 0;
	// glGetIntegerv(GL_NUM_EXTENSIONS, &no_of_extensions);
	// printf("Number of available extensions %d\n", no_of_extensions);
	// for (int i = 0; i < no_of_extensions; ++i)
	// 	printf("\t%s\n", glGetStringi(GL_EXTENSIONS, i));

	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	int samples = 4;
	float quadVerts[] = {
		-1.0, -1.0,     0.0, 0.0,
		-1.0, 1.0,      0.0, 1.0,
		1.0, -1.0,      1.0, 0.0,

		1.0, -1.0,      1.0, 0.0,
		-1.0, 1.0,      0.0, 1.0,
		1.0, 1.0,       1.0, 1.0
	};

	GLuint	vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint	vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

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

	char	*faces[6] = {"res/cubemaps/GalleryRight.jpg",
						 "res/cubemaps/GalleryLeft.jpg",
						 "res/cubemaps/GalleryTop.jpg",
						 "res/cubemaps/GalleryBottom.jpg",
						 "res/cubemaps/GalleryBack.jpg",
						 "res/cubemaps/GalleryFront.jpg"};

	int	im_width, im_height, im_channels;
	uint8_t	*data;

	for (uint32_t i = 0; i < 6; i++) {
		data = stbi_load(faces[i], &im_width, &im_height, &im_channels, 0);
		if (data)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
						 im_width, im_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else
			printf("Cubemap tex failed to load at path: %s\n", faces[i]);
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	GLuint	earth;
	glGenTextures(1, &earth);
	glBindTexture(GL_TEXTURE_2D, earth);
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("res/textures/Earthmap.jpg", &im_width, &im_height, &im_channels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, im_width, im_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		printf("Earth tex failed to load\n");
	stbi_image_free(data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	GLuint	organic;
	glGenTextures(1, &organic);
	glBindTexture(GL_TEXTURE_2D, organic);
	data = stbi_load("res/textures/Organic4.jpg", &im_width, &im_height, &im_channels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, im_width, im_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		printf("Organic tex failed to load\n");
	stbi_image_free(data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// GLuint	pebble;
	// glGenTextures(1, &pebble);
	// glBindTexture(GL_TEXTURE_2D, pebble);
	// data = stbi_load("res/textures/Organic4.jpg", &im_width, &im_height, &im_channels, 0);
	// if (data) {
	// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, im_width, im_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	// 	glGenerateMipmap(GL_TEXTURE_2D);
	// } else
	// 	printf("Organic tex failed to load\n");
	// stbi_image_free(data);

	// GLuint64 handle = glGetTextureHandleARB(0);
	// if (handle == 0)
	// 	printf("Error! Handle returned null\n");

	printf("Bindless texture is %savailable\n", glGetTextureHandleARB ? "" : "not ");

	uint32_t	buf_a_program = create_shader_program("res/shaders/buffer_a.vert", "res/shaders/buffer_a.frag");
	uint32_t	image_program = create_shader_program("res/shaders/image.vert", "res/shaders/image.frag");
	if (!buf_a_program && !image_program && dprintf(2, "Error\n"))
		exit(-1);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(buf_a_program);
	glUniform1i(glGetUniformLocation(buf_a_program, "iChannel0"), 0);
	glUniform1i(glGetUniformLocation(buf_a_program, "organic"), 1);
	glUniform1i(glGetUniformLocation(buf_a_program, "earth"), 2);

	glUniform1i(glGetUniformLocation(buf_a_program, "skybox"), 3);

	glUseProgram(image_program);
	glUniform1i(glGetUniformLocation(image_program, "iChannel0"), 0);

	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, iChannel0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, organic);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, earth);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glViewport(0, 0, (int)(width / DENSITY), (int)(height/ DENSITY));
		glUseProgram(buf_a_program);
		glUniform2fv(glGetUniformLocation(buf_a_program, "iResolution"), 1, (float []){WIDTH, HEIGHT});
		glUniform1i(glGetUniformLocation(buf_a_program, "iFrame"), (int)(glfwGetTime() * 60));
		glUniform1f(glGetUniformLocation(buf_a_program, "iTime"), glfwGetTime());
		glDrawArrays(GL_TRIANGLES, 0, 6);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		glUseProgram(image_program);
		glUniform2fv(glGetUniformLocation(image_program, "iResolution"), 1, (float []){WIDTH, HEIGHT});
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// glUseProgram(program);
		// glUniform2fv(glGetUniformLocation(program, "iResolution"), 1, (float []){width, height});
		// glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}
