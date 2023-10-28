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

int	save_object(t_object *obj, int type, char *str)
{
	int		i;
	int		j;
	int		n;

	obj->type = type >> 27 & 0x1f;
	i = 9;
	while (--i >= 0)
	{
		j = 3;
		while (((type >> 3 * i) & 0x7) && --j >= 0)
		{
			n = populate_buffer(str, (float *)&obj->pos
					+ ((int)fmax(i, 5) % 2 + (2 - !i) * (i < 5)) * 3
					+ (i > 3 && i < 7) * 3 + (i < 4 && i) * (3 - i) + (2 - j),
					type, (type >> 3 * i) & 0x7);
			j -= j * (((type >> 3 * i) & 0x7) > NORM);
			if (n <= 0 || (j > 0 && str[n] != ','))
				return (1);
			str += n + (j > 0 && str[n] == ',');
		}
	}
	return (0 || ((type >> 21 & 0x7) == NORM && vec3_length(obj->axis) == 0.f));
}

int	save_objects(t_scene *scene, char *str)
{
	int	type;

	while (*str == ' ' || *str == '\t')
		str++;
	type = obj_type(str);
	str += (type != 0) + (type >= SPHERE);
	if (type == AMBI && !scene->ambient.type)
		return (save_object(&scene->ambient, type, str));
	if (type == CAMERA && !scene->camera.type)
		return (save_object(&scene->camera, type, str));
	if (type == PTLGHT && !scene->light.type)
		return (save_object(&scene->light, type, str));
	if (type >= SPHERE && scene->n_obj < MAX_SIZE)
		return (save_object(&scene->objects[scene->n_obj++], type, str));
	return (1);
}