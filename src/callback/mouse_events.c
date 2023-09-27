/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mouse_events.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/28 08:51:18 by rraharja          #+#    #+#             */
/*   Updated: 2023/09/27 10:11:06 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "callbacks.h"
#include "MLX42/MLX42_Int.h"

/**
 * Callback function used to handle scrolling, scaling (zooming) the map view
 * either up or down depending on the scroll direction.
 *
 * @param	xdelta	The mouse x delta
 * @param	ydelta	The mouse y delta
 * @param	param	Pointer to fdf struct
 */
void	scroll_hook(double xdelta, double ydelta, void *param)
{
	t_rtx	*rtx;

	rtx = (t_rtx *)param;
	rtx->scene.camera.param[3] -= (float)ydelta * S_SENSITIVITY;
	if (rtx->scene.camera.param[3] < 1.f)
		rtx->scene.camera.param[3] = 1.f;
	if (rtx->scene.camera.param[3] > 179.f)
		rtx->scene.camera.param[3] = 179.f;
	rtx->refresh = true;
	(void)xdelta;
}

/**
 * Callback function used to handle raw mouse movement, either translating,
 * rotating or changing the elevation of the map depending on the button pressed
 * and whether the CTRL button is pressed.
 *
 * @param	xpos	The mouse x position
 * @param	ypos	The mouse y position
 * @param	param	Pointer to fdf struct
 */
void	cursor_hook(double xpos, double ypos, void *param)
{
	double	*delta;
	t_rtx	*rtx;

	rtx = (t_rtx *)param;
	delta = (double []){xpos - rtx->pos[0], ypos - rtx->pos[1]};
	if (mlx_is_mouse_down(rtx->mlx, MLX_MOUSE_BUTTON_LEFT))
	{
		rtx->yaw += M_SENSITIVITY * ((delta[0] < 0) - (delta[0] > 0));
		rtx->pitch += M_SENSITIVITY * ((delta[1] > 0) - (delta[1] < 0));
		if (fabs(rtx->pitch) > M_PI_2 - 0.01f)
			rtx->pitch += M_SENSITIVITY * ((rtx->pitch < 0) - (rtx->pitch > 0));
		rtx->scene.camera.axis.x = cos(rtx->yaw) * cos(rtx->pitch);
		rtx->scene.camera.axis.y = sin(rtx->pitch);
		rtx->scene.camera.axis.z = sin(rtx->yaw) * cos(rtx->pitch);
		rtx->refresh = true;
	}
	rtx->pos[0] = xpos;
	rtx->pos[1] = ypos;
}

/**
 * Callback function used to handle mouse actions, storing where the mouse is
 * clicked.
 *
 * @param	button	The mouse button/key pressed
 * @param	action	The mouse action that took place
 * @param	mods	The modifier keys pressed together with the mouse key
 * @param	param	Pointer to fdf struct
 */
void	mouse_hook(mouse_key_t button, action_t action,
	modifier_key_t mods, void *param)
{
	t_rtx	*rtx;

	rtx = (t_rtx *)param;
	if (action == MLX_PRESS)
		mlx_get_mouse_pos(rtx->mlx, &rtx->pos[0], &rtx->pos[1]);
	(void)button;
	(void)mods;
}
