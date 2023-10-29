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

int	save_material(t_rtx *rtx, t_material *mat, int type, char *s)
{
	// if (type == QMAP)
	// 	rtx->tex[0] = setup_cubemap(s);
	// return (rtx->tex[0] == 0);
}

int	save_object(t_rtx *rtx, t_object *obj, int type, char *s)
{
	int	i;
	int	j;
	int	n;

	obj->type = type >> 27 & 0x1f;
	i = 9;
	while (--i > 0 || (!i && type < SPHERE && type != QMAP))
	{
		j = 3;
		while (((type >> 3 * i) & 0x7) && --j >= 0)
		{
			n = populate_buffer(s, (float *)&obj->pos + (i > 3 && i < 7) * 3
					+ ((int)fmax(i, 5) % 2 + (i < 5)) * 4 + (i < 4 && i)
					* (3 - i) + (2 - j), type, (type >> 3 * i) & 0x7);
			j -= j * (((type >> 3 * i) & 0x7) > NORM);
			if (n <= 0 || (j > 0 && s[n] != ','))
				return (1);
			s += n + (j > 0 && s[n] == ',');
		}
	}
	while (ft_isspace(*s) && (i || !i && *s != '\n'))
		s++;
	return (((type >> 21 & 0x7) == NORM && vec3_length(obj->axis) == 0.f)
		|| (i && *s) || (!i && save_material(rtx, &rtx->scene.materials[
					(int)fmax(rtx->scene.n_obj - 1, 0)], type, s)));
}

int	save_objects(t_rtx *rtx, t_scene *scene, char *s)
{
	int			type;
	t_object	o;

	while (ft_isspace(*s) && *s != '\n')
		s++;
	type = obj_type(s);
	s += (type != 0) + (type == QMAP || type >= PTLGHT);
	if (type == SIZE && !save_object(rtx, &o, type, s))
	{
		if (o.param[0] < 0 || o.param[0] > 3500
			|| o.param[1] < 0 || o.param[1] > 3500)
			memcpy(o.param, (float []){WIDTH, HEIGHT}, 2 * sizeof(float));
		memcpy(rtx->size, (int []){o.param[0], o.param[1]}, 2 * sizeof(int));
		return (0);
	}
	if (type == QMAP && !save_object(rtx, &o, type, s))
	{
		rtx->cb_intensity = o.param[3];
		return (0);
	}
	if (type == AMBI && !scene->ambient.type)
		return (save_object(rtx, &scene->ambient, type, s));
	if (type == CAMERA && !scene->camera.type)
		return (save_object(rtx, &scene->camera, type, s));
	if ((type == PTLGHT || type == SPLGHT) && scene->n_light < MAX_SIZE)
	{
		scene->lights[scene->n_light] = (t_object){.i_cone = 180.f,
			.o_cone = 180.f};
		return (save_object(rtx, &scene->lights[scene->n_light++], type, s));
	}
	if (type >= SPHERE && scene->n_obj < MAX_SIZE)
		return (save_object(rtx, &scene->objects[scene->n_obj++], type, s));
	return (*s && *s != '\n');
}
