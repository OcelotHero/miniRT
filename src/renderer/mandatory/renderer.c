/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   renderer.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/10 12:46:13 by rraharja          #+#    #+#             */
/*   Updated: 2023/10/30 06:10:16 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "renderer.h"

void	set_px_color(mlx_image_t *img, t_cam *cam, int *pos, t_vec3 color)
{
	uint8_t	*px;

	color = vec3_scale(1.f / (float) cam->samples, color);
	px = img->pixels + (pos[0] + pos[1] * img->width) * sizeof(int32_t);
	*px = (int)(fmin(fmax(sqrt(color.r), 0), 1) * 255.f);
	px++;
	*px = (int)(fmin(fmax(sqrt(color.g), 0), 1) * 255.f);
	px++;
	*px = (int)(fmin(fmax(sqrt(color.b), 0), 1) * 255.f);
	px++;
	*px = 255;
}

t_vec3	r_color(t_ray r, t_scene *scene)
{
	float		weight;
	t_ray		s_ray;
	t_vec3		l_dir;
	t_vec3		l_col;
	t_record	rec[2];

	if (hit(r, (float []){.001f, 1.f / 0.f}, &rec[0], scene))
	{
		l_dir = vec3_normalize(vec3_elem_op(scene->light.pos, '-', rec[0].p));
		s_ray = (t_ray){rec[0].p, l_dir};
		weight = fmin(fmax(vec3_dot(rec[0].n, l_dir), 0), 1);
		if (hit(s_ray, (float []){.001f, 1.f / 0.f}, &rec[1], scene)
			&& vec3_length(vec3_elem_op(scene->light.pos, '-', s_ray.ori))
				> rec[1].t)
			weight = 0;
		l_col = vec3_scale(weight * scene->light.param[3], scene->light.color);
		l_col = vec3_elem_op(l_col, '+',
				vec3_scale(scene->ambient.param[3], scene->ambient.color));
		return (vec3_elem_op(l_col, '*', rec[0].c));
	}
	weight = .5f * (vec3_normalize(r.dir).y + 1.f);
	return (vec3_elem_op(vec3_scale(1.f - weight, (t_vec3){1, 1, 1}), '+',
		vec3_scale(weight, (t_vec3){.5f, .7f, 1.f})));
}

void	render(t_cam *cam, t_scene *scene, mlx_image_t *img)
{
	int		i[3];
	t_ray	r;
	t_vec3	color;

	i[0] = -1;
	while (++i[0] < cam->i_height)
	{
		i[1] = -1;
		while (++i[1] < cam->i_width)
		{
			color = (t_vec3){0, 0, 0};
			i[2] = -1;
			while (++i[2] < cam->samples)
			{
				r = (t_ray){cam->center, vec3_normalize(vec3_elem_op(
							vec3_elem_op(cam->p00_loc, '-', cam->center), '+',
							vec3_elem_op(vec3_scale(i[1] + (float)rand()
									/ RAND_MAX - .5f, cam->delta_u),
								'+', vec3_scale(i[0] + (float)rand()
									/ RAND_MAX - .5f, cam->delta_v))))};
				color = vec3_elem_op(color, '+', r_color(r, scene));
			}
			set_px_color(img, cam, (int []){i[1], i[0]}, color);
		}
	}
}
