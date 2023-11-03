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

int	save_object(t_object *obj, char *s, int *n)
{
	int	i;
	int	j;
	int	l;

	obj->type = n[1] >> 27 & 0x1f;
	i = 9;
	while (--i >= 0)
	{
		j = 3;
		while (((n[1] >> 3 * i) & 0x7) && --j >= 0)
		{
			l = populate_buffer(&s[*n], (float *)&obj->pos + (2 - j) + (3 - i)
					* (i < 4 && i) + ((int)fmax(i, 5) % 2 + (2 - !i) * (i < 5))
					* 3 + (i > 3 && i < 7) * 3, n[1], (n[1] >> 3 * i) & 0x7);
			j -= j * (((n[1] >> 3 * i) & 0x7) > NORM);
			if (l <= 0 || (j && s[*n + l] != ',')
				|| (i == 7 && !j && vec3_length(obj->axis) == 0.f))
				return (*n);
			*n += l + (j && s[*n + l] == ',');
		}
	}
	while (ft_isspace(s[*n]))
		(*n)++;
	return (*n * (s[*n] != '\0'));
}

int	save_objects(t_scene *scene, char *s)
{
	int	n;
	int	type;

	n = 0;
	while (ft_isspace(s[n]) && s[n] != '\n')
		n++;
	type = obj_type(&s[n]);
	n += (type != 0) + (type >= SPHERE);
	if (type == AMBI && !scene->ambient.type)
		return (save_object(&scene->ambient, s, (int []){n, type}));
	if (type == CAMERA && !scene->camera.type)
		return (save_object(&scene->camera, s, (int []){n, type}));
	if (type == PTLGHT && !scene->light.type)
		return (save_object(&scene->light, s, (int []){n, type}));
	if (type >= SPHERE && scene->n_obj < MAX_SIZE)
		return (save_object(&scene->objects[scene->n_obj++], s,
				(int []){n, type}));
	return (-(s[n] && s[n] != '\n'));
}
