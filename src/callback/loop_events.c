/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loop_events_b.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/06 07:59:57 by rraharja          #+#    #+#             */
/*   Updated: 2023/07/06 07:59:57 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "callbacks.h"

/**
 * Helper function to set OpenGL int uniform value.
 *
 * @param	pg	Specifies the program object to be queried
 * @param	id	The name of the uniform variable whose location is to be queried
 * @param	a	The value to be assigned to the uniform
 */
static void	set_int(uint32_t pg, char *id, int a)
{
	glUniform1i(glGetUniformLocation(pg, id), a);
}

/**
 * Helper function to set OpenGL vec2 uniform entries.
 *
 * @param	pg	Specifies the program object to be queried
 * @param	id	The name of the uniform variable whose location is to be queried
 * @param	a	The value to be assigned to the first entry of the uniform
 * @param	b	The value to be assigned to the second entry of the uniform
 */
static void	set_float2(uint32_t pg, char *id, float a, float b)
{
	glUniform2f(glGetUniformLocation(pg, id), a, b);
}

/**
 * Callback function used to render each frame, calling the corresponding shader
 * program and setting up all uniforms needed to render the frame.
 *
 * @param	fdf		Pointer to fdf struct
 */
void	loop_hook(void *param)
{
	t_rtx	*rtx;

	rtx = (t_rtx *)param;
	rtx->delta_time = glfwGetTime() - rtx->last_frame;
	rtx->last_frame += rtx->delta_time;
	if (rtx->refresh)
	{
		glBufferSubData(GL_UNIFORM_BUFFER, 16, 64, &rtx->scene.camera);
		rtx->start_time = glfwGetTime();
		rtx->refresh = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, rtx->framebuffer);
	glViewport(0, 0, rtx->size[0], rtx->size[1]);
	glUseProgram(rtx->buf_a_program);
	set_float2(rtx->buf_a_program, "iResolution", rtx->size[0], rtx->size[1]);
	set_int(rtx->buf_a_program, "iFrame",
		(int)((glfwGetTime() - rtx->start_time) * 60));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, rtx->size[0] * DENSITY, rtx->size[1] * DENSITY);
	glUseProgram(rtx->image_program);
	set_float2(rtx->image_program, "iResolution", rtx->size[0] * DENSITY, rtx->size[1] * DENSITY);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
