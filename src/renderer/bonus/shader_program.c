/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shader_program.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 05:26:47 by rraharja          #+#    #+#             */
/*   Updated: 2023/11/19 10:05:58 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "renderer_b.h"

/**
 * Opens the shader source code file and returns it as string.
 *
 * @param	source	Path to shader source code
 * @return	Source code string
 */
static char	*shader_source(const char *source)
{
	char	*buffer;
	long	length;
	FILE	*f;

	f = fopen(source, "r");
	if (!f && error_msg(E_FOP, strerror(errno)))
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
	if (!buffer && error_msg(E_MLC, strerror(errno)))
		return (NULL);
	return (buffer);
}

/**
 * Creates, compiles, and returns a shader object of the given type from the
 * given shader source code.
 *
 * @param	type	Shader type to compile
 * @param	source	Path to shader source code
 * @return	Reference ID to shader object or 0 if an error occurs
 */
static uint32_t	create_shader(int32_t type, const char *source)
{
	int			success;
	char		*source_code;
	char		log[512];
	uint32_t	shader;

	source_code = shader_source(source);
	if (!source_code)
		return (0);
	shader = glCreateShader(type);
	if (!shader)
		return (0);
	glShaderSource(shader, 1, (const GLchar * const *)&source_code, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, log);
		if (type == GL_VERTEX_SHADER)
			error_msg(E_VSC, log);
		else
			error_msg(E_FSC, log);
	}
	free(source_code);
	return ((success != 0) * shader);
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
	vshader = create_shader(GL_VERTEX_SHADER, vert);
	fshader = create_shader(GL_FRAGMENT_SHADER, frag);
	if (program && vshader && fshader)
	{
		glAttachShader(program, vshader);
		glAttachShader(program, fshader);
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(program, 512, NULL, log);
			error_msg(E_SHL, log);
		}
	}
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	return ((success != 0 && vshader && fshader) * program);
}
