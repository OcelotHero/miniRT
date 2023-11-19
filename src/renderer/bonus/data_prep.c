/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   data_prep.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/19 09:45:39 by rraharja          #+#    #+#             */
/*   Updated: 2023/11/19 09:45:39 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "renderer_b.h"

/**
 * Creates shader programs for different view modes.
 *
 * @param	fdf	Pointer to fdf struct
 * @return	Whether the operation was successful
 */
int	setup_shader_program(t_rtx *rtx)
{
	rtx->buf_a_program = create_shader_program("res/shaders/buffer_a.vert",
			"res/shaders/buffer_a.frag");
	rtx->image_program = create_shader_program("res/shaders/image.vert",
			"res/shaders/image.frag");
	return (!rtx->buf_a_program || !rtx->image_program);
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
			return (1);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	rtx->tex[0].id = fbuffer_tex;
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
	return (!buf[0] || !buf[1] || !buf[2]);
}

int	load_scene_to_buffer(t_rtx *rtx)
{
	int			i;
	t_object	*obj;

	glBufferSubData(GL_UNIFORM_BUFFER, 0, 45136, &rtx->scene.n_obj);
	glBufferSubData(GL_UNIFORM_BUFFER, 45312, 4, &rtx->scene.n_light);
	glBufferSubData(GL_UNIFORM_BUFFER, 45328, 16, &rtx->scene.ambient.param);
	i = -1;
	while (++i < rtx->scene.n_light)
	{
		obj = &rtx->scene.lights[i];
		glBufferSubData(GL_UNIFORM_BUFFER, 45344 + i * 48, 48, &obj->pos);
	}
	return (0);
}
