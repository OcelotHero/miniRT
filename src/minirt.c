/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minirt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/30 06:08:14 by rraharja          #+#    #+#             */
/*   Updated: 2023/10/30 06:08:14 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

#include "maths.h"
#include "renderer.h"
#include "types_common.h"
#include "types_mandatory.h"
#include "parser_common.h"
#include "parser_mandatory.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>

void	setup_camera(t_scene *scene, t_cam *cam)
{
	float	v_width;
	float	v_height;
	t_vec3	v_u;
	t_vec3	v_v;
	t_vec3	v_up_left;

	v_width = 2.f * tan(scene->camera.param[3] * M_PI / 360.f);
	v_height = v_width * cam->i_height / cam->i_width;
	cam->center = scene->camera.pos;
	cam->vecs[2] = vec3_scale(-1.f, vec3_normalize(scene->camera.axis));
	cam->vecs[0] = vec3_elem_op((t_vec3){0, 1, 0}, 'x', cam->vecs[2]);
	cam->vecs[1] = vec3_elem_op(cam->vecs[2], 'x', cam->vecs[0]);
	v_u = vec3_scale(v_width, cam->vecs[0]);
	v_v = vec3_scale(-v_height, cam->vecs[1]);
	cam->delta_u = vec3_scale(1.f / cam->i_width, v_u);
	cam->delta_v = vec3_scale(1.f / cam->i_height, v_v);
	v_up_left = vec3_elem_op(cam->center, '-', vec3_elem_op(cam->vecs[2], '+',
				vec3_scale(0.5f, vec3_elem_op(v_u, '+', v_v))));
	cam->p00_loc = vec3_elem_op(v_up_left, '+',
			vec3_scale(.5f, vec3_elem_op(cam->delta_u, '+', cam->delta_v)));
}

int	setup_scene(t_scene *scene, t_cam *cam, int narg, char **args)
{
	int		fd;
	char	*line;
	char	*endl;

	if (narg != 2)
		return (printf("Invalid argument\n"));
	fd = open(args[1], O_RDONLY);
	if (fd < 0)
		return (printf("Unable to open %s\n", args[1]));
	line = get_next_line(fd);
	while (line)
	{
		if (save_objects(scene, line))
		{
			endl = ft_strchr(line, '\n');
			if (endl)
				*endl = '\0';
			return (printf("Error parsing\n\t%s\n", line));
		}
		free(line);
		line = get_next_line(fd);
	}
	setup_camera(scene, cam);
	return (0);
}

void	key_hook(mlx_key_data_t keydata, void *param)
{
	mlx_t		*mlx;

	mlx = (mlx_t *)param;
	if (keydata.key == MLX_KEY_ESCAPE && keydata.action == MLX_PRESS)
	{
		mlx_terminate(mlx);
		exit(0);
	}
}

int	main(int narg, char **args)
{
	t_cam		cam;
	t_scene		scene;
	mlx_t		*mlx;
	mlx_image_t	*img;

	scene = (t_scene){0};
	cam = (t_cam){WIDTH, HEIGHT, SAMPLE};
	if (setup_scene(&scene, &cam, narg, args))
		return (1);
	mlx = mlx_init(cam.i_width, cam.i_height, "miniRT", true);
	img = mlx_new_image(mlx, cam.i_width, cam.i_height);
	render(&cam, &scene, img);
	mlx_image_to_window(mlx, img, 0, 0);
	mlx_key_hook(mlx, key_hook, mlx);
	mlx_loop(mlx);
}
