/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_mandatory.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 09:52:46 by rraharja          #+#    #+#             */
/*   Updated: 2023/10/28 09:52:46 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser_mandatory.h"

int	save_object(t_object *obj, int typ, char *s)
{
	int		i;
	int		j;
	int		n;

	obj->type = typ >> 27 & 0x1f;
	i = 9;
	while (--i >= 0)
	{
		j = 3;
		while (((typ >> 3 * i) & 0x7) && --j >= 0)
		{
			n = populate_buffer(s, (float *)&obj->pos + (i > 3 && i < 7) * 3
					+ ((int)fmax(i, 5) % 2 + (2 - !i) * (i < 5)) * 3 + (3 - i)
					* (i < 4 && i) + (2 - j), typ, (typ >> 3 * i) & 0x7);
			j -= j * (((typ >> 3 * i) & 0x7) > NORM);
			if (n <= 0 || (j > 0 && s[n] != ','))
				return (1);
			s += n + (j > 0 && s[n] == ',');
		}
	}
	while ((*s >= '\t' && *s <= '\r') || *s == ' ')
		s++;
	return (*s || ((typ >> 21 & 0x7) == NORM && vec3_length(obj->axis) == 0.f));
}

int	save_objects(t_scene *scene, char *s)
{
	int	typ;

	while (*s == ' ' || *s == '\t')
		s++;
	typ = obj_type(s);
	s += (typ != 0) + (typ >= SPHERE);
	if (typ == AMBI && !scene->ambient.type)
		return (save_object(&scene->ambient, typ, s));
	if (typ == CAMERA && !scene->camera.type)
		return (save_object(&scene->camera, typ, s));
	if (typ == PTLGHT && !scene->light.type)
		return (save_object(&scene->light, typ, s));
	if (typ >= SPHERE && scene->n_obj < MAX_SIZE)
		return (save_object(&scene->objects[scene->n_obj++], typ, s));
	return (1);
}
