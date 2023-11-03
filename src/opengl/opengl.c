/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   opengl.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 05:26:47 by rraharja          #+#    #+#             */
/*   Updated: 2023/11/03 22:50:22 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "maths.h"
#include "utils.h"
#include "callback.h"
#include "types_c.h"
#include "types_b.h"
#include "parser_c.h"
#include "parser_b.h"

#include "stb_image.h"
#include "stb_image_resize.h"
#include "MLX42/MLX42_Int.h"

#include <glob.h>
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
	glShaderSource(shader, 1, (const GLchar * const *)&shader_source, NULL);
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

#define SP	0x7
#define PL	0x8
#define	CY	0x9
#define	BX	0xa
#define	CN	0xb
#define	TR	0xc
#define	QD	0xd
#define	DS	0xe

// #define SP	0x00800
// #define CY	0x02000
// #define CN	0x04000
// #define BX	0x08000
// #define PL	0x01000
// #define QD	0x10000
// #define DS	0x20000
// #define TR	0x40000

void	set_scene_geometry(t_scene *scene)
{
	int	n = 0;

	// back wall
	scene->objects[n++] = (t_object){.type = QD,
		.pos	= {   0.00f,   0.00f,  -5.00f},
		.axis	= {  12.60f,   0.00f,  -5.00f},
		.param	= {   0.00f,  12.60f,  -5.00f,   0.00f}};
	// floor
	scene->objects[n++] = (t_object){.type = QD,
		.pos 	= {   0.00f, -12.45f,   0.00f},
		.axis	= {  12.60f, -12.45f,   0.00f},
		.param	= {   0.00f, -12.45f,  -5.00f,   0.00f}};
	// ceiling
	scene->objects[n++] = (t_object){.type = QD,
		.pos 	= {   0.00f,  12.50f,   0.00f},
		.axis	= {   0.00f,  12.50f,  -5.00f},
		.param	= {  12.60f,  12.50f,   0.00f,   0.00f}};
	// left wall
	scene->objects[n++] = (t_object){.type = QD,
		.pos	= { -12.50f,   0.00f,   0.00f},
		.axis	= { -12.50f,  12.60f,   0.00f},
		.param	= { -12.50f,   0.00f,  -5.00f,   0.00f}};
	// right wall
	scene->objects[n++] = (t_object){.type = QD,
		.pos	= {  12.50f,   0.00f,   0.00f},
		.axis	= {  12.50f,  12.60f,   0.00f},
		.param	= {  12.50f,   0.00f,  -5.00f,   0.00f}};
	// light
	scene->objects[n++] = (t_object){.type = QD,
		.pos	= {   0.00f,  12.40f,   0.00f},
		.axis	= {   5.00f,  12.40f,   0.00f},
		.param	= {   0.00f,  12.40f,  -2.50f,   0.00f}};

	// picture
	scene->objects[n++] = (t_object){.type = QD,
		.pos	= {   0.00f,   0.00f,  -4.90f},
		.axis	= {   0.00f,   3.50f,  -4.90f},
		.param	= {   5.50f,   0.00f,  -4.90f,   0.00f}};
	// strip pattern
	scene->objects[n++] = (t_object){.type = QD,
		.pos	= {   0.00f,  -8.50f,  -4.90f},
		.axis	= {   0.00f,  -5.00f,  -4.90f},
		.param	= {   5.50f,  -8.50f,  -4.90f,   0.00f}};

	// spheres of varying specular roughness
	scene->objects[n++] = (t_object){.type = SP,
		.pos		= { -10.00f,   0.00f,  -3.00f},
		.param[0]	= 1.75f};
	scene->objects[n++] = (t_object){.type = SP,
		.pos		= {  -5.00f,   0.00f,  -3.00f},
		.param[0]	= 1.75f};
	scene->objects[n++] = (t_object){.type = SP,
		.pos		= {   0.00f,   0.00f,  -3.00f},
		.param[0]	= 1.75f};
	scene->objects[n++] = (t_object){.type = SP,
		.pos		= {   5.00f,   0.00f,  -3.00f},
		.param[0]	= 1.75f};
	scene->objects[n++] = (t_object){.type = SP,
		.pos		= {  10.00f,   0.00f,  -3.00f},
		.param[0]	= 1.75f};

	scene->objects[n++] = (t_object){.type = CY,
		.pos		= {  -7.00f,  -9.90f,   0.00f},
		.axis		= {   0.00f,   0.90f,   0.00f},
		.param		= {   2.00f,   2.50f,   0.00f,   0.00f}};
	scene->objects[n++] = (t_object){.type = CY,
		.pos		= {  -7.00f, -10.00f,  -1.00f},
		.axis		= {   0.00f,   0.90f,   0.00f},
		.param		= {   0.50f,   2.00f,   0.00f,   0.00f}};
	scene->objects[n++] = (t_object){.type = CY,
		.pos		= {  -7.00f, -10.00f,   1.00f},
		.axis		= {   0.00f,   0.90f,   0.00f},
		.param		= {   0.50f,   2.00f,   0.00f,   0.00f}};

	scene->objects[n++] = (t_object){.type = CN,
		.pos		= {   0.00f,  -8.00f,   0.00f},
		.axis		= {   0.00f,   0.50f,   0.50f},
		.param		= {   1.00f,   2.00f,   2.50f,   0.00f}};

	scene->objects[n++] = (t_object){.type = SP,
		.pos		= {   8.00f,  -8.00f,   2.00f},
		.param[0]	= 4.f};

	scene->objects[n++] = (t_object){.type = BX,
		.pos		= {   8.00f,   8.00f,   0.00f},
		.axis		= {   0.00f,   1.00f,   0.00f},
		.param		= {   2.00f,   2.50f,   2.00f,  30.00f}};

	scene->n_obj = n;
}

void	set_scene_material(t_scene *scene)
{
	int	n = 0;

	// back wall
	scene->materials[n++] = (t_material){.albedo = {   0.7f,   0.7f,   0.7f},
										 .ior = 1.0f};
	// floor
	scene->materials[n++] = (t_material){.albedo	= {   1.0f,   1.0f,   1.0f},
										 .texture	= {  64.0f, 128.0f,  -1.0f},
										 .ior = 1.0f};
	// ceiling
	scene->materials[n++] = (t_material){.albedo = {   0.7f,   0.7f,   0.7f},
										 .ior = 1.0f};
	// left wall
	scene->materials[n++] = (t_material){.albedo = {   0.7f,   0.1f,   0.1f},
										 .ior = 1.0f};
	// right wall
	scene->materials[n++] = (t_material){.albedo = {   0.1f,   0.7f,   0.1f},
										 .ior = 1.0f};
	// light
	scene->materials[n++] = (t_material){.emissive = {   1.0f,   0.9f,   0.5f},
										 .intensity = 5.0f,
										 .ior = 1.0f};

	// picture
	scene->materials[n++] = (t_material){.albedo	= {   1.0f,   1.0f,   1.0f},
										 .texture	= {   0.0f,   0.0f,   2.0f},
										 .ior = 1.0f};
	// strip pattern
	scene->materials[n++] = (t_material){.albedo	= {   1.0f,   1.0f,   1.0f},
										 .texture	= {  64.0f,   0.0f,  -1.0f},
										 .ior = 1.0f};

	// spheres of varying specular roughness
	scene->materials[n++] = (t_material){.albedo			= {   1.0f,   1.0f,   1.0f},
										 .spec_color		= {   0.3f,   1.0f,   0.3f},
										 .spec_chance		= 1.0f,
										 .spec_roughness	= 0.0f,
										 .ior = 1.0f};
	scene->materials[n++] = (t_material){.albedo			= {   1.0f,   1.0f,   1.0f},
										 .spec_color		= {   0.3f,   1.0f,   0.3f},
										 .spec_chance		= 1.0f,
										 .spec_roughness	= 0.25f,
										 .ior = 1.0f};
	scene->materials[n++] = (t_material){.albedo			= {   1.0f,   1.0f,   1.0f},
										 .spec_color		= {   0.3f,   1.0f,   0.3f},
										 .spec_chance		= 1.0f,
										 .spec_roughness	= 0.5f,
										 .ior = 1.0f};
	scene->materials[n++] = (t_material){.albedo			= {   1.0f,   1.0f,   1.0f},
										 .spec_color		= {   0.3f,   1.0f,   0.3f},
										 .spec_chance		= 1.0f,
										 .spec_roughness	= 0.75f,
										 .ior = 1.0f};
	scene->materials[n++] = (t_material){.albedo			= {   1.0f,   1.0f,   1.0f},
										 .spec_color		= {   0.3f,   1.0f,   0.3f},
										 .spec_chance		= 1.0f,
										 .spec_roughness	= 1.0f,
										 .ior = 1.0f};


	scene->materials[n++] = (t_material){.albedo			= {   0.9f,   0.9f,   0.9f},
										 .spec_color		= {   0.9f,   0.9f,   0.9f},
										 .refr_color		= {   0.0f,   0.0f,   0.0f},
										 .spec_chance		= 0.02f,
										 .refr_chance		= 0.96f,
										 .ior = 1.5f};
	scene->materials[n++] = (t_material){.albedo			= {   0.9f,   0.9f,   0.9f},
										 .spec_color		= {   0.9f,   0.9f,   0.9f},
										 .refr_color		= {   0.1f,   0.4f,   1.0f},
										 .spec_chance		= 0.0002f,
										 .refr_chance		= 1.00f,
										 .intensity			= 1.0f,
										 .ior = 1.5f};
	scene->materials[n++] = (t_material){.albedo			= {   0.9f,   0.9f,   0.9f},
										 .spec_color		= {   0.9f,   0.9f,   0.9f},
										 .refr_color		= {   1.0f,   0.4f,   0.0f},
										 .spec_chance		= 0.0002f,
										 .refr_chance		= 1.00f,
										 .intensity			= 1.0f,
										 .ior = 1.3f};

	scene->materials[n++] = (t_material){.albedo			= {   0.9f,   0.9f,   0.9f},
										 .spec_color		= {   0.9f,   0.9f,   0.9f},
										 .refr_color		= {   0.8f,   0.0f,   0.0f},
										 .spec_chance		= 0.02f,
										 .refr_chance		= 1.00f,
										 .intensity			= 1.0f,
										 .ior = 1.2f};

	scene->materials[n++] = (t_material){.albedo			= {   1.0f,   1.0f,   1.0f},
										 .texture			= {   0.2f,   0.0f,   3.0f},
										 .normal_map		= {   0.2f,   0.0f,  16.0f,   3.0f},
										 .spec_color		= {   1.0f,   1.0f,   1.0f},
										 .spec_chance		= 0.5f,
										 .ior = 1.0f};

	scene->materials[n++] = (t_material){.albedo			= {   0.8f,   0.3f,   0.9f},
										 .texture			= {  32.0f,  32.0f,  -1.0f},
										 .spec_color		= {   0.8f,   0.3f,   0.9f},
										 .spec_chance		= 0.5f,
										 .ior = 1.0f};
}

void	set_scene_light(t_rtx *rtx, t_scene *scene)
{
	save_objects(rtx, scene, "A 0.1 229.5,229.5,0");
	save_objects(rtx, scene, "ls -12.4,-10,0 1,0,0 35 55 1.0 255,229.5,127.5");
	save_objects(rtx, scene, "lp -12.4,10,0 1.0 25.5,25.5,229.5");
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
		glBufferSubData(GL_UNIFORM_BUFFER, 45344 + i * 48, 48, &obj->pos);
	}
	return (0);
}

int	setup_and_load_scene(t_rtx *rtx)
{
	t_scene	*scene;

	scene = &rtx->scene;
	save_objects(rtx, scene, "C 0,0,30 0,0,-1 90");
	set_scene_geometry(scene);
	set_scene_material(scene);
	set_scene_light(rtx, scene);
	move_scene_to_buffer(scene);
	return (0);
}

int	setup_buffer_objects(t_rtx *rtx)
{
	GLuint	buf[3];
	float	*v_scr;

	glGenVertexArrays(1, &buf[0]);
	glBindVertexArray(buf[0]);
	v_scr = (float []){-1.f, -1.f, -1.f, 1.f, 1.f, -1.f,
		1.f, -1.f, -1.f, 1.f, 1.f, 1.f};
	glGenBuffers(1, &buf[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buf[1]);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(*v_scr), v_scr, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
		2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenBuffers(1, &buf[2]);
	glBindBuffer(GL_UNIFORM_BUFFER, buf[2]);
	glBufferData(GL_UNIFORM_BUFFER, 57632, NULL, GL_STATIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, buf[2], 0, 45136);
	glBindBufferRange(GL_UNIFORM_BUFFER, 1, buf[2], 45312, 12320);
	glUniformBlockBinding(rtx->buf_a_program,
		glGetUniformBlockIndex(rtx->buf_a_program, "Objects"), 0);
	glUniformBlockBinding(rtx->buf_a_program,
		glGetUniformBlockIndex(rtx->buf_a_program, "Lights"), 1);
	return (!buf[0] || !buf[1] || !buf[2] || setup_and_load_scene(rtx));
}

/**
 * Creates shader programs for different view modes.
 *
 * @param	fdf	Pointer to fdf struct
 * @return	Whether the operation was successful
 */
static int	setup_shader_program(t_rtx *rtx)
{
	rtx->buf_a_program = create_shader_program("res/shaders/buffer_a.vert",
			"res/shaders/buffer_a.frag");
	rtx->image_program = create_shader_program("res/shaders/image.vert",
			"res/shaders/image.frag");
	return (!rtx->buf_a_program || !rtx->image_program);
}

int	setup_texture(char *path)
{
	int		width;
	int		height;
	int		channels;
	GLuint	tex;
	uint8_t	*data;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load(path, &width, &height, &channels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB + (channels == 4), width, height,
			0, GL_RGB + (channels == 4), GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		return (printf("%s tex failed to load!\n", path) & 0);
	stbi_image_free(data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
	return (tex);
}

int	setup_cubemap_textures(char *dir, char *path, int *im_prop, uint8_t **data)
{
	static char	*faces[] = {"Front", "Back", "Top", "Bottom", "Left", "Right"};
	int			ret;
	char		*wild;
	glob_t		gs;

	wild = strchr(dir, '*');
	if (!wild)
		return (printf("Missing wildcard in filepath '%s'!\n", dir));
	memcpy(path, dir, wild - dir);
	path[wild - dir] = '\0';
	sprintf(path, "%s%s.*", path, faces[im_prop[0]]);
	ret = glob(path, 0, NULL, &gs);
	if (ret == GLOB_NOMATCH)
		return (printf("%s cubemap tex not found!\n", path));
	if (ret)
		return (printf("Glob error %s\n", strerror(errno)));
	memcpy(path, gs.gl_pathv[0], strlen(gs.gl_pathv[0]) + 1);
	globfree(&gs);
	*data = stbi_load(path, &im_prop[0], &im_prop[1], &im_prop[2], 0);
	if (!data)
		return (printf("%s cubemap tex failed to load!\n", path));
	return (0);
}

int	setup_cubemap(char *dir)
{
	int		i;
	int		im_prop[3];
	char	path[512];
	uint8_t	*data;
	GLuint	cubemap;

	glGenTextures(1, &cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	i = -1;
	while (++i < 6)
	{
		im_prop[0] = i;
		if (setup_cubemap_textures(dir, path, im_prop, &data))
			return (0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_RGB + (im_prop[2] == 4), im_prop[0], im_prop[1], 0,
			GL_RGB + (im_prop[2] == 4), GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return (cubemap);
}

int	setup_framebuffer(t_rtx *rtx)
{
	GLuint	fbuffer_tex;

	glGenFramebuffers(1, &rtx->framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, rtx->framebuffer);
	{
		glGenTextures(1, &fbuffer_tex);
		glBindTexture(GL_TEXTURE_2D, fbuffer_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, rtx->size[0], rtx->size[1],
			0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, fbuffer_tex, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return (printf("Framebuffer is not complete!\n") & 0);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	return (fbuffer_tex);
}

int	load_texture(t_rtx *rtx)
{
	int		i;
	char	buffer[10];

	glUseProgram(rtx->buf_a_program);
	glUniform1i(glGetUniformLocation(rtx->buf_a_program, "framebuffer"), 0);
	glUniform1i(glGetUniformLocation(rtx->buf_a_program, "skybox"), 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rtx->tex[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, rtx->tex[1]);
	i = 1;
	while (++i < 16)
	{
		sprintf(buffer, "tex%02d", i);
		glUniform1i(glGetUniformLocation(rtx->buf_a_program, buffer), i);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, rtx->tex[i]);
	}
	glUseProgram(rtx->image_program);
	glUniform1i(glGetUniformLocation(rtx->image_program, "framebuffer"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rtx->tex[0]);
	return (0);
}

int	main(int narg, char *args[])
{
	t_rtx	rtx = {.size = {WIDTH, HEIGHT}};
	t_vec3	v;

	rtx.mlx = mlx_init(rtx.size[0], rtx.size[1], "miniRT", true);
	if (setup_shader_program(&rtx))
		return (cleanup(&rtx, 1));
	rtx.tex[0] = setup_framebuffer(&rtx);
	rtx.tex[1] = setup_cubemap("res/cubemaps/Gallery*");
	rtx.tex[2] = setup_texture("res/textures/Organic4.jpg");
	rtx.tex[3] = setup_texture("res/textures/Earthmap.jpg");
	if (!rtx.tex[0] || !rtx.tex[1] || !rtx.tex[2] || !rtx.tex[3])
		return (cleanup(&rtx, 1));
	load_texture(&rtx);
	if (setup_buffer_objects(&rtx))
		return (cleanup(&rtx, 1));
	v = rtx.scene.camera.axis;
	rtx.pitch = M_PI_2 - acos(v.y / sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
	rtx.yaw = ((v.z >= 0) - (v.z < 0)) * acos(v.x / sqrt(v.x * v.x + v.z * v.z));
	mlx_key_hook(rtx.mlx, key_hook, &rtx);
	mlx_mouse_hook(rtx.mlx, mouse_hook, &rtx);
	mlx_cursor_hook(rtx.mlx, cursor_hook, &rtx);
	mlx_scroll_hook(rtx.mlx, scroll_hook, &rtx);
	mlx_resize_hook(rtx.mlx, resize_hook, &rtx);
	mlx_loop_hook(rtx.mlx, loop_hook, &rtx);
	rtx.refresh = true;
	mlx_loop(rtx.mlx);
}
