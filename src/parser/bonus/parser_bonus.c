/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 09:52:43 by rraharja          #+#    #+#             */
/*   Updated: 2023/10/28 09:52:43 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser_bonus.h"

int	save_material(t_material *mat, int type, char *s)
{
	return (1);
}

int	save_object(t_object *obj, t_material *mat, int typ, char *s)
{
	int		i;
	int		j;
	int		n;

	obj->type = typ >> 27 & 0x1f;
	i = 9;
	while (--i > 0 || (!i && typ < SPHERE && typ != QMAP))
	{
		j = 3;
		while (((typ >> 3 * i) & 0x7) && --j >= 0)
		{
			n = populate_buffer(s, (float *)&obj->pos + (i > 3 && i < 7) * 3
					+ ((int)fmax(i, 5) % 2 + (i < 5)) * 4 + (i < 4 && i)
					* (3 - i) + (2 - j), typ, (typ >> 3 * i) & 0x7);
			j -= j * (((typ >> 3 * i) & 0x7) > NORM);
			if (n <= 0 || (j > 0 && s[n] != ','))
				return (1);
			s += n + (j > 0 && s[n] == ',');
		}
	}
	if (!i)
		return (save_material(mat, typ, s));
	while ((*s >= '\t' && *s <= '\r') || *s == ' ')
		s++;
	return (*s || ((typ >> 21 & 0x7) == NORM && vec3_length(obj->axis) == 0.f));
}

int	save_objects(t_rtx *rtx, t_scene *scene, char *s)
{
	int			typ;
	t_object	obj;

	while (*s == ' ' || *s == '\t')
		s++;
	typ = obj_type(s);
	s += (typ != 0) + (typ == QMAP || typ >= PTLGHT);
	if (typ == SIZE)
	{
		if (save_object(&obj, NULL, typ, s))
			memcpy(rtx->size, (int []){WIDTH, HEIGHT}, 2 * sizeof(int));
		else
			memcpy(rtx->size,
				(int []){obj.param[0], obj.param[1]}, 2 * sizeof(int));
		return (0);
	}
	// if (typ == QMAP)
	if (typ == AMBI && !scene->ambient.type)
		return (save_object(&scene->ambient, NULL, typ, s));
	if (typ == CAMERA && !scene->camera.type)
		return (save_object(&scene->camera, NULL, typ, s));
	if ((typ == PTLGHT || typ == SPLGHT) && scene->n_light < MAX_SIZE)
	{
		scene->lights[scene->n_light] = (t_object){.i_cone = 180.f,
			.o_cone = 180.f};
		return (save_object(&scene->lights[scene->n_light++], NULL, typ, s));
	}
	if (typ >= SPHERE && scene->n_obj < MAX_SIZE)
		return (save_object(&scene->objects[scene->n_obj],
				&scene->materials[scene->n_obj++], typ, s));
	return (1);
}
