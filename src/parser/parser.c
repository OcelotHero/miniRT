/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/09 15:18:10 by rraharja          #+#    #+#             */
/*   Updated: 2023/09/26 12:54:33 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "get_next_line.h"

#include "types.h"

#include <fcntl.h>
#include <unistd.h>

int	n_atof(char *str, float *val)
{
	int		i;
	int		sign;
	double	point;
	double	total;

	i = 0;
	total = 0;
	while ((str[i] >= '\t' && str[i] <= '\r') || str[i] == ' ')
		i++;
	sign = (str[i] == '+') - (str[i] == '-');
	i += (sign != 0);
	sign += sign == 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		total *= 10;
		total += str[i++] - '0';
	}
	point = (str[i] == '.' && ++i);
	while (str[i] >= '0' && str[i] <= '9')
	{
		point /= 10;
		total += (str[i++] - '0') * point;
	}
	*val = sign * total;
	return (i);
}

int	obj_type(char *str)
{
	if (!ft_strncmp(str, "A", 1) && (*(str + 1) == ' ' || *(str + 1) == '\t'))
		return (AMBI);
	if (!ft_strncmp(str, "C", 1) && (*(str + 1) == ' ' || *(str + 1) == '\t'))
		return (CAMERA);
	if (!ft_strncmp(str, "L", 1) && (*(str + 1) == ' ' || *(str + 1) == '\t'))
		return (LIGHT);
	if (!ft_strncmp(str, "sp", 2) && (*(str + 2) == ' ' || *(str + 2) == '\t'))
		return (SPHERE);
	if (!ft_strncmp(str, "pl", 2) && (*(str + 2) == ' ' || *(str + 2) == '\t'))
		return (PLANE);
	if (!ft_strncmp(str, "cy", 2) && (*(str + 2) == ' ' || *(str + 2) == '\t'))
		return (CYLND);
	return (INVAL);
}

int	populate_buffer(char *str, float *mem, int type)
{
	int	n;

	n = n_atof(str, mem);
	if (((type & A_PCNT) && (*mem < 0 || *mem > 1))
		|| ((type & A_FOV) && (*mem < 0 || *mem > 180))
		|| ((type & A_NORM) && (*mem < -1 || *mem > 1))
		|| ((type & A_COL) && (*mem < 0 || *mem > 255)))
		return (-1);
	return (n);
}

int	save_object(t_object *object, int type, char *str)
{
	int		i[3];
	float	*mem;

	object->type = type;
	i[0] = -1;
	while (++i[0] < 7)
	{
		if (!(type & (1 << i[0])))
			continue ;
		i[1] = 3;
		while (--i[1] >= 0)
		{
			i[2] = ((1 << i[0]) == A_NORM) + ((1 << i[0]) == A_COL) * 2;
			mem = &((*(&(object->pos) + i[2])).e[2 - i[1]]);
			if ((1 << i[0]) & (A_PCNT | A_FLT1 | A_FLT2 | A_FOV))
				mem = &object->param[i[0] - 2];
			i[1] -= i[1] * (((A_PCNT | A_FLT1 | A_FLT2 | A_FOV)
					& (1 << i[0])) > 0);
			i[2] = populate_buffer(str, mem, (1 << i[0]));
			if (i[2] == -1 || (i[1] > 0 && str[i[2]] != ','))
				return (1);
			str += i[2] + (i[2] > 0);
		}
	}
	return (0);
}

int	save_objects(t_scene *scene, char *str)
{
	int	type;

	while (*str == ' ' || *str == '\t')
		str++;
	type = obj_type(str);
	str += (type != 0) + (type > LIGHT);
	if ((type & (AMBI & ~0xff)) && !scene->ambient.type)
		return (save_object(&scene->ambient, type, str));
	if ((type & (CAMERA & ~0xff)) && !scene->camera.type)
		return (save_object(&scene->camera, type, str));
	if ((type & (LIGHT & ~0xff)) && !scene->light.type)
		return (save_object(&scene->light, type, str));
	if ((type & ((SPHERE | PLANE | CYLND) & ~0xff)) && scene->n_obj < MAX_SIZE)
		return (save_object(&scene->objects[scene->n_obj++], type, str));
	return (1);
}

// int	main()
// {
// 	t_scene	scene;
// 	char	*line;

// 	scene = (t_scene){0, {0}, {0}, {0}};
// 	int fd = open("scene.rt", O_RDONLY);

// 	line = get_next_line(fd);
// 	while(line)
// 	{
// 		if (save_objects(&scene, line))
// 			return (1);
// 		free(line);
// 		line = get_next_line(fd);
// 	}
// 	return (0);
// }