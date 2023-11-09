/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_events.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/06 08:01:04 by rraharja          #+#    #+#             */
/*   Updated: 2023/07/06 08:01:04 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "callback.h"

/**
 * Callback function used to handle window resizing to maintain correct aspect
 * ratio.
 *
 * @param	width	The new width of the window
 * @param	height	The new height of the window
 * @param	param	Pointer to fdf struct
 */
void	resize_hook(int32_t width, int32_t height, void *param)
{
	t_rtx	*rtx;

	rtx = (t_rtx *)param;
	memcpy(rtx->size, (int []){width, height}, 2 * sizeof(int));
	glBindTexture(GL_TEXTURE_2D, rtx->tex[0].id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, rtx->size[0], rtx->size[1],
			0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	rtx->refresh = true;
}
