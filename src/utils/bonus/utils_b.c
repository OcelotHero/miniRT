/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_b.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/17 01:10:57 by rraharja          #+#    #+#             */
/*   Updated: 2023/11/17 01:10:57 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils_b.h"

/**
 * Helper function to set OpenGL int uniform value.
 *
 * @param	pg	Specifies the program object to be queried
 * @param	id	The name of the uniform variable whose location is to be queried
 * @param	a	The value to be assigned to the uniform
 */
void	set_int(uint32_t pg, char *id, int a)
{
	glUniform1i(glGetUniformLocation(pg, id), a);
}

/**
 * Helper function to set OpenGL float uniform entries.
 *
 * @param	pg	Specifies the program object to be queried
 * @param	id	The name of the uniform variable whose location is to be queried
 * @param	a	The value to be assigned to the uniform
 */
void	set_float(uint32_t pg, char *id, float a)
{
	glUniform1f(glGetUniformLocation(pg, id), a);
}

/**
 * Helper function to set OpenGL vec2 uniform entries.
 *
 * @param	pg	Specifies the program object to be queried
 * @param	id	The name of the uniform variable whose location is to be queried
 * @param	a	The value to be assigned to the first entry of the uniform
 * @param	b	The value to be assigned to the second entry of the uniform
 */
void	set_float2(uint32_t pg, char *id, float a, float b)
{
	glUniform2f(glGetUniformLocation(pg, id), a, b);
}