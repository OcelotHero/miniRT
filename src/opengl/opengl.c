/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   opengl.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 05:26:47 by rraharja          #+#    #+#             */
/*   Updated: 2023/11/09 07:23:38 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

#include "maths.h"
#include "utils.h"
#include "opengl.h"
#include "callback.h"
#include "types_c.h"
#include "types_b.h"
#include "parser_c.h"
#include "parser_b.h"

#include "MLX42/MLX42_Int.h"

#include <fcntl.h>

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
	set_scene_material(scene);
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

int	load_texture(t_rtx *rtx)
{
	int		i;
	char	buffer[10];

	glUseProgram(rtx->buf_a_program);
	glUniform1i(glGetUniformLocation(rtx->buf_a_program, "framebuffer"), 0);
	glUniform1i(glGetUniformLocation(rtx->buf_a_program, "skybox"), 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rtx->tex[0].id);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, rtx->tex[1].id);
	i = 1;
	while (++i < 16)
	{
		sprintf(buffer, "tex%02d", i);
		glUniform1i(glGetUniformLocation(rtx->buf_a_program, buffer), i);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, rtx->tex[i].id);
	}
	glUseProgram(rtx->image_program);
	glUniform1i(glGetUniformLocation(rtx->image_program, "framebuffer"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rtx->tex[0].id);
	return (0);
}

int	parse_error(int m, int n, char *line)
{
	int		i;
	char	*endl;
	char	buf[512];

	n *= n > 0;
	endl = ft_strchr(line, '\n');
	if (endl)
	*endl = '\0';
	i = -1;
	while (++i < n || ft_isspace(line[i]))
		buf[i] = ft_isspace(line[i]) * line[i] + !ft_isspace(line[i]) * ' ';
	ft_memcpy(&buf[i], "^", 2);
	return (error_msg(E_PRS, m, line, buf));
}

int	setup_scene(t_rtx *rtx, int fd)
{
	int		i;
	int		n;
	char	*line;

	i = 0;
	line = get_next_line(fd);
	while (line && ++i)
	{
		rtx->err[0] = '\0';
		n = save_objects(rtx, &rtx->scene, line);
		if (n)
			parse_error(i, n, line);
		if (rtx->err[0])
			error_msg(rtx->err);
		free(line);
		if (n)
		{
			get_next_line(-fd);
			return (1);
		}
		line = get_next_line(fd);
	}
	return (0);
}

int	setup_config(t_rtx *rtx)
{
	if (setup_shader_program(rtx) || !setup_framebuffer(rtx)
		|| !setup_texture(rtx, "res/textures/Organic4.jpg", 0)
		|| !setup_texture(rtx, "res/textures/Earthmap.jpg", 0)
		|| load_texture(rtx) || setup_buffer_objects(rtx))
		return (1);
	if (!rtx->size[0] && !rtx->size[1])
		memcpy(rtx->size, (int []){WIDTH, HEIGHT}, 2 * sizeof(int));
	return (0);
}

int	main(int narg, char **args)
{
	int		fd;
	t_rtx	rtx;
	t_vec3	v;

	rtx = (t_rtx){0};
	if (narg != 2)
		return (error_msg(E_USG));
	fd = open(args[1], O_RDONLY);
	if (fd < 0)
		return (error_msg(E_OPN, args[1]));
	rtx.mlx = mlx_init(WIDTH, HEIGHT, "miniRT", true);
	if (setup_scene(&rtx, fd))
		return (close(fd), mlx_terminate(rtx.mlx), 1);
	close(fd);
	mlx_set_window_size(rtx.mlx, rtx.size[0], rtx.size[1]);
	if (setup_config(&rtx))
		return (mlx_terminate(rtx.mlx), 1);
	v = rtx.scene.camera.axis;
	rtx.pitch = M_PI_2 - acos(v.y / sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
	rtx.yaw = ((v.z > 0) - (v.z < 0)) * acos(v.x / sqrt(v.x * v.x + v.z * v.z));
	mlx_key_hook(rtx.mlx, key_hook, &rtx);
	mlx_mouse_hook(rtx.mlx, mouse_hook, &rtx);
	mlx_cursor_hook(rtx.mlx, cursor_hook, &rtx);
	mlx_scroll_hook(rtx.mlx, scroll_hook, &rtx);
	mlx_resize_hook(rtx.mlx, resize_hook, &rtx);
	mlx_loop_hook(rtx.mlx, loop_hook, &rtx);
	rtx.refresh = true;
	mlx_loop(rtx.mlx);
}
