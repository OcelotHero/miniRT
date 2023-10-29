/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_m.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 09:52:46 by rraharja          #+#    #+#             */
/*   Updated: 2023/10/28 09:52:46 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser_m.h"

int	save_object(t_object *obj, int type, char *s)
{
	int	i;
	int	j;
	int	n;

	obj->type = type >> 27 & 0x1f;
	i = 9;
	while (--i >= 0)
	{
		j = 3;
		while (((type >> 3 * i) & 0x7) && --j >= 0)
		{
			n = populate_buffer(s, (float *)&obj->pos + (i > 3 && i < 7) * 3
					+ ((int)fmax(i, 5) % 2 + (2 - !i) * (i < 5)) * 3 + (3 - i)
					* (i < 4 && i) + (2 - j), type, (type >> 3 * i) & 0x7);
			j -= j * (((type >> 3 * i) & 0x7) > NORM);
			if (n <= 0 || (j > 0 && s[n] != ','))
				return (1);
			s += n + (j > 0 && s[n] == ',');
		}
	}
	while (ft_isspace(*s))
		s++;
	return (*s
		|| ((type >> 21 & 0x7) == NORM && vec3_length(obj->axis) == 0.f));
}

int	save_objects(t_scene *scene, char *s)
{
	int	type;

	while (ft_isspace(*s) && *s != '\n')
		s++;
	type = obj_type(s);
	s += (type != 0) + (type >= SPHERE);
	if (type == AMBI && !scene->ambient.type)
		return (save_object(&scene->ambient, type, s));
	if (type == CAMERA && !scene->camera.type)
		return (save_object(&scene->camera, type, s));
	if (type == PTLGHT && !scene->light.type)
		return (save_object(&scene->light, type, s));
	if (type >= SPHERE && scene->n_obj < MAX_SIZE)
		return (save_object(&scene->objects[scene->n_obj++], type, s));
	return (*s && *s != '\n');
}
