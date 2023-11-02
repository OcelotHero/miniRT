/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_b.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 09:52:43 by rraharja          #+#    #+#             */
/*   Updated: 2023/10/28 09:52:43 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser_b.h"

int	save_material(t_rtx *rtx, t_material *mat, char *s, int *n)
{
	// if (type == QMAP)
	// 	rtx->tex[0] = setup_cubemap(s);
	// return (rtx->tex[0] == 0);
}

int	save_object(t_rtx *rtx, t_object *obj, char *s, int *n)
{
	int	i;
	int	j;
	int	m;

	obj->type = n[1] >> 27 & 0x1f;
	i = 9;
	while (--i > 0 || (!i && n[1] < SPHERE && n[1] != QMAP))
	{
		j = 3;
		while (((n[1] >> 3 * i) & 0x7) && --j >= 0)
		{
			m = populate_buffer(&s[*n], (float *)&obj->pos + (2 - j) + (3 - i)
					* (i < 4 && i) + ((int)fmax(i, 5) % 2 + (i < 5)) * 4
					+ (i > 3 && i < 7) * 3, n[1], (n[1] >> 3 * i) & 0x7);
			j -= j * (((n[1] >> 3 * i) & 0x7) > NORM);
			if (m <= 0 || (j && s[*n + m] != ',')
				|| (i == 7 && !j && vec3_length(obj->axis) == 0.f))
				return (1);
			*n += m + (j && s[*n + m] == ',');
		}
	}
	while (ft_isspace(s[*n]) && (i || !i && s[*n] != '\n'))
		(*n)++;
	return (*n * ((!i && save_material(rtx, &rtx->scene.materials[(int)fmax(
							rtx->scene.n_obj - 1, 0)], s, n)) || (i && s[*n])));
}

int	save_objects(t_rtx *rtx, t_scene *scene, char *s)
{
	int			n;
	int			type;
	t_object	o;

	n = 0;
	while (ft_isspace(s[n]) && s[n] != '\n')
		n++;
	type = obj_type(&s[n]);
	n += (type != 0) + (type == QMAP || type >= PTLGHT);
	if (type == SIZE && !save_object(rtx, &o, s, (int []){n, type}))
	{
		if (o.param[0] < 0 || o.param[0] > 3500
			|| o.param[1] < 0 || o.param[1] > 3500)
			memcpy(o.param, (float []){WIDTH, HEIGHT}, 2 * sizeof(float));
		memcpy(rtx->size, (int []){o.param[0], o.param[1]}, 2 * sizeof(int));
		return (0);
	}
	if (type == QMAP && !save_object(rtx, &o, s, (int []){n, type}))
	{
		rtx->cb_intensity = o.param[3];
		return (0);
	}
	if (type == AMBI && !scene->ambient.type)
		return (save_object(rtx, &scene->ambient, s, (int []){n, type}));
	if (type == CAMERA && !scene->camera.type)
		return (save_object(rtx, &scene->camera, s, (int []){n, type}));
	if ((type == PTLGHT || type == SPLGHT) && scene->n_light < MAX_SIZE)
	{
		scene->lights[scene->n_light] = (t_object){.i_cone = 180.f,
			.o_cone = 180.f};
		return (save_object(rtx, &scene->lights[scene->n_light++], s,
				(int []){n, type}));
	}
	if (type >= SPHERE && scene->n_obj < MAX_SIZE)
		return (save_object(rtx, &scene->objects[scene->n_obj++], s,
				(int []){n, type}));
	return (-(s[n] && s[n] != '\n'));
}
