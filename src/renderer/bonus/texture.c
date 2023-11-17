/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 04:06:43 by rraharja          #+#    #+#             */
/*   Updated: 2023/11/09 04:06:43 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "opengl.h"

#include "stb_image.h"
#include "stb_image_resize.h"

int	setup_2d_texture(t_rtx *rtx, char *path)
{
	int		width;
	int		height;
	int		channels;
	GLuint	tex;
	uint8_t	*im;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	stbi_set_flip_vertically_on_load(true);
	im = stbi_load(path, &width, &height, &channels, 0);
	if (im)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB + (channels == 4), width, height,
			0, GL_RGB + (channels == 4), GL_UNSIGNED_BYTE, im);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		return (snprintf(rtx->err, sizeof(rtx->err), E_TLD, path) & 0);
	stbi_image_free(im);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
	return (tex);
}

int	setup_cubemap_texture(t_rtx *rtx, char *dir, int *prop, uint8_t **im)
{
	static char	*faces[] = {"Front", "Back", "Top", "Bottom", "Left", "Right"};
	int			ret;
	char		buf[512];
	char		*wild;
	glob_t		gs;

	snprintf(buf, sizeof(buf), "%s", dir);
	wild = strchr(buf, '*');
	if (!wild)
		return (snprintf(rtx->err, sizeof(rtx->err), E_QWL, dir));
	snprintf(wild, sizeof(buf) - (wild - buf), "%s.*", faces[prop[0]]);
	ret = glob(buf, 0, NULL, &gs);
	if (ret == GLOB_NOMATCH)
		return (snprintf(rtx->err, sizeof(rtx->err), E_QNF, buf));
	if (ret)
		return (snprintf(rtx->err, sizeof(rtx->err), E_QGL, strerror(errno)));
	snprintf(buf, sizeof(buf), "%s", gs.gl_pathv[0]);
	globfree(&gs);
	*im = stbi_load(buf, &prop[0], &prop[1], &prop[2], 0);
	if (!im)
		return (snprintf(rtx->err, sizeof(rtx->err), E_QLD, buf));
	return (0);
}

int	setup_cubemap(t_rtx *rtx, char *dir)
{
	int		i;
	int		prop[3];
	uint8_t	*im;
	GLuint	cubemap;

	glGenTextures(1, &cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	i = -1;
	while (++i < 6)
	{
		prop[0] = i;
		if (setup_cubemap_texture(rtx, dir, prop, &im))
			return (0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_RGB + (prop[2] == 4), prop[0], prop[1], 0,
			GL_RGB + (prop[2] == 4), GL_UNSIGNED_BYTE, im);
		stbi_image_free(im);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return (cubemap);
}

int	setup_texture(t_rtx *rtx, char *path, int type)
{
	int	i;

	if (type == QMAP)
	{
		rtx->tex[1].id = setup_cubemap(rtx, path);
		snprintf(rtx->tex[1].path, sizeof(rtx->tex[1].path), "%s", path);
		return (rtx->tex[1].id != 0);
	}
	i = 1;
	while (++i < 16 && rtx->tex[i].id)
		if (!strcmp(rtx->tex[i].path, path))
			return (i);
	if (i >= 16)
		return (snprintf(rtx->err, sizeof(rtx->err), E_TMX) & 0);
	rtx->tex[i].id = setup_2d_texture(rtx, path);
	snprintf(rtx->tex[i].path, sizeof(rtx->tex[i].path), "%s", path);
	if (rtx->tex[i].id)
		return (i);
	return (0);
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
			return (snprintf(rtx->err, sizeof(rtx->err), E_FNC) & 0);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	rtx->tex[0].id = fbuffer_tex;
	return (1);
}
