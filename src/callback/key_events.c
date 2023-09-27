/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   key_events.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 05:47:32 by rraharja          #+#    #+#             */
/*   Updated: 2023/07/05 05:47:32 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "callbacks.h"

enum e_movement {
	FWD, BWD, RGT, LFT, INC, DEC
};

/**
 * A helper function to handle key presses:
 * 		WASD/Arrow keys	To increment or decrement parameters used in mode 3 - 5
 *
 * @param	keydata	The callback data, containing info on key
 * @param	fdf		Pointer to fdf struct
 */
static void	move_camera(t_rtx *rtx, enum e_movement mv)
{
	float		dist;
	t_object	*cam;

	cam = &rtx->scene.camera;
	dist = MV_SPEED * rtx->delta_time;
	if (mv == FWD)
		cam->pos = vec4_elem_op(cam->pos, '+', vec4_scale(dist, cam->axis));
	else if (mv == BWD)
		cam->pos = vec4_elem_op(cam->pos, '-', vec4_scale(dist, cam->axis));
	else if (mv == RGT)
		cam->pos = vec4_elem_op(cam->pos, '-', vec4_scale(dist,
			vec4_elem_op(cam->axis, 'x', (t_vec4){0.f, 1.f, 0.f, 0.f})));
	else if (mv == LFT)
		cam->pos = vec4_elem_op(cam->pos, '+', vec4_scale(dist,
			vec4_elem_op(cam->axis, 'x', (t_vec4){0.f, 1.f, 0.f, 0.f})));
	else if (mv == INC)
		cam->pos.y += dist;
	else if (mv == DEC)
		cam->pos.y -= dist;
	rtx->refresh = true;
}

/**
 * Callback function used to handle key presses:
 * 		WASD/Arrow keys	To move camera position in the desired direction
 * 		Q				To decrement camera elevation
 * 		E				To increment camera elevation
 *
 * @param	keydata	The callback data, containing info on key
 * @param	param	Pointer to fdf struct
 */
void	key_hook(mlx_key_data_t keydata, void *param)
{
	t_rtx	*rtx;

	rtx = (t_rtx *)param;
	if (keydata.key == MLX_KEY_ESCAPE && keydata.action == MLX_PRESS)
		exit(cleanup(rtx, 0));
	if ((keydata.key == MLX_KEY_UP || keydata.key == MLX_KEY_W)
		&& (keydata.action == MLX_PRESS || keydata.action == MLX_REPEAT))
		move_camera(rtx, FWD);
	else if ((keydata.key == MLX_KEY_DOWN || keydata.key == MLX_KEY_S)
		&& (keydata.action == MLX_PRESS || keydata.action == MLX_REPEAT))
		move_camera(rtx, BWD);
	else if ((keydata.key == MLX_KEY_RIGHT || keydata.key == MLX_KEY_D)
		&& (keydata.action == MLX_PRESS || keydata.action == MLX_REPEAT))
		move_camera(rtx, RGT);
	else if ((keydata.key == MLX_KEY_LEFT || keydata.key == MLX_KEY_A)
		&& (keydata.action == MLX_PRESS || keydata.action == MLX_REPEAT))
		move_camera(rtx, LFT);
	else if (keydata.key == MLX_KEY_E
		&& (keydata.action == MLX_PRESS || keydata.action == MLX_REPEAT))
		move_camera(rtx, INC);
	else if (keydata.key == MLX_KEY_Q
		&& (keydata.action == MLX_PRESS || keydata.action == MLX_REPEAT))
		move_camera(rtx, DEC);
}
