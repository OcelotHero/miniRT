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

int	save_material(t_material *mat, int type, char *str)
{
	return (1);
}

int	save_object(t_object *obj, t_material *mat, int type, char *str)
{
	int		i;
	int		j;
	int		n;

	obj->type = type >> 27 & 0x1f;
	i = 9;
	while (--i > 0 || (!i && type >= AMBI && type <= SPLGHT))
	{
		j = 3;
		while (((type >> 3 * i) & 0x7) && --j >= 0)
		{
			n = populate_buffer(str, (float *)&obj->pos
					+ ((int)fmax(i, 5) % 2 + (i < 5)) * 4
					+ (i > 3 && i < 7) * 3 + (i < 4 && i) * (3 - i) + (2 - j),
					type, (type >> 3 * i) & 0x7);
			j -= j * (((type >> 3 * i) & 0x7) > NORM);
			if (n <= 0 || (j > 0 && str[n] != ','))
				return (1);
			str += n + (j > 0 && str[n] == ',');
		}
	}
	if (!i)
		return (save_material(mat, type, str));
	return (0 || ((type >> 21 & 0x7) == NORM && vec3_length(obj->axis) == 0.f));
}

int	save_objects(t_scene *scene, char *str)
{
	int	type;

	while (*str == ' ' || *str == '\t')
		str++;
	type = obj_type(str);
	str += (type != 0) + (type == QMAP || type >= PTLGHT);
	if (type == AMBI && !scene->ambient.type)
		return (save_object(&scene->ambient, NULL, type, str));
	if (type == CAMERA && !scene->camera.type)
		return (save_object(&scene->camera, NULL, type, str));
	if ((type == PTLGHT || type == SPLGHT) && scene->n_light < MAX_SIZE)
	{
		scene->lights[scene->n_light] = (t_object){.i_cone = 180.f,
			.o_cone = 180.f};
		return (save_object(&scene->lights[scene->n_light++], NULL, type, str));
	}
	if (type >= SPHERE && scene->n_obj < MAX_SIZE)
		return (save_object(&scene->objects[scene->n_obj],
				&scene->materials[scene->n_obj++], type, str));
	return (1);
}