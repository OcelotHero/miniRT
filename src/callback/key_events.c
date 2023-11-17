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

#include "callback.h"

enum e_movement {
	FWD = 1 << 0,
	BWD = 1 << 1,
	RGT = 1 << 2,
	LFT = 1 << 3,
	INC = 1 << 4,
	DEC = 1 << 5
};

/**
 * A helper function to handle key presses:
 * 		WASD/Arrow keys	To increment or decrement parameters used in mode 3 - 5
 *
 * @param	keydata	The callback data, containing info on key
 * @param	fdf		Pointer to fdf struct
 */
static void	move_camera(t_rtx *rtx, int movement)
{
	float		dist;
	t_object	*cam;

	cam = &rtx->scene.camera;
	dist = MV_SPEED * DENSITY * rtx->delta_time;
	if (movement & FWD)
		cam->pos = vec3_elem_op(cam->pos, '+', vec3_scale(dist, cam->axis));
	if (movement & BWD)
		cam->pos = vec3_elem_op(cam->pos, '-', vec3_scale(dist, cam->axis));
	if (movement & RGT)
		cam->pos = vec3_elem_op(cam->pos, '+', vec3_scale(dist,
					vec3_elem_op(cam->axis, 'x', (t_vec3){0, 1, 0})));
	if (movement & LFT)
		cam->pos = vec3_elem_op(cam->pos, '-', vec3_scale(dist,
					vec3_elem_op(cam->axis, 'x', (t_vec3){0, 1, 0})));
	if (movement & INC)
		cam->pos.y += dist;
	if (movement & DEC)
		cam->pos.y -= dist;
	rtx->refresh = movement > 0;
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
	static int	movement = 0;
	if (keydata.key == MLX_KEY_ESCAPE && keydata.action == MLX_PRESS)
		mlx_terminate(((t_rtx *)param)->mlx);
	if (keydata.key == MLX_KEY_ESCAPE && keydata.action == MLX_PRESS)
		exit(0);
	if ((keydata.key == MLX_KEY_UP || keydata.key == MLX_KEY_W)
		&& (keydata.action == MLX_PRESS || keydata.action == MLX_RELEASE))
		movement ^= FWD;
	else if ((keydata.key == MLX_KEY_DOWN || keydata.key == MLX_KEY_S)
		&& (keydata.action == MLX_PRESS || keydata.action == MLX_RELEASE))
		movement ^= BWD;
	else if ((keydata.key == MLX_KEY_RIGHT || keydata.key == MLX_KEY_D)
		&& (keydata.action == MLX_PRESS || keydata.action == MLX_RELEASE))
		movement ^= RGT;
	else if ((keydata.key == MLX_KEY_LEFT || keydata.key == MLX_KEY_A)
		&& (keydata.action == MLX_PRESS || keydata.action == MLX_RELEASE))
		movement ^= LFT;
	else if (keydata.key == MLX_KEY_E
		&& (keydata.action == MLX_PRESS || keydata.action == MLX_RELEASE))
		movement ^= INC;
	else if (keydata.key == MLX_KEY_Q
		&& (keydata.action == MLX_PRESS || keydata.action == MLX_RELEASE))
		movement ^= DEC;
	move_camera((t_rtx *)param, movement);
}
