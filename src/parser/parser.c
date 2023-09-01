/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/09 15:18:10 by rraharja          #+#    #+#             */
/*   Updated: 2023/08/12 09:23:13 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "get_next_line.h"

#include "types.h"

#include <fcntl.h>
#include <unistd.h>

/**
 * Performs similarly to atof function, whereby it converts the initial portion
 * of valid numerical string pointed to by str to float and returning the number
 * of characters parsed. The converted value is stored into the memory buffer
 * pointed to by val.
 *
 * @param	str	String to be converted to float
 * @param	val	Pointer to store the converted value
 * @return	The number of characters parsed from the string
 */
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

/**
 * Returns the type of object refered to by the line currently parsed, pointed
 * to by str, by checking the identifier.
 *
 * The following are valid identifiers and its corresponding type:
 * 	A		- AMBI
 * 	C		- CAMERA
 * 	L		- LIGHT
 * 	sp		- SPHERE
 * 	pl		- PLANE
 * 	cy		- CYLND
 * 	default	- INVAL
 *
 * @param	str	The line to be parsed
 * @return	Object type
 */
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

/**
 * Saves the numerical string pointed to by str to memory buffer pointed to by
 * mem, checking the validity of the value returned depending on the type of
 * attribute to be stored. Returns the number of characters parsed from the
 * string if the value stored in the memory buffer is valid, -1 otherwise.
 *
 * The following are the attributes and its valid value range:
 * 	A_PCNT	- percentage value 		[0, 1]
 * 	A_FOV	- field of view 		[0, 180]
 * 	A_NORM	- normalized value 		[-1, 1]
 * 	A_COL	- rgb value				[0, 255]
 * 	A_POS	- position vector		(-inf, inf)
 * 	A_FLTx	- floating point number	(-inf, inf)
 *
 * @param	str		The line to be parsed
 * @param	mem		The memory buffer to store the value to
 * @param	type	The type of attribute to be stored
 * @return	The number of characters parsed or -1 on error
 */
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

/**
 * Saves object properties to t_object struct by parsing the given string
 * pointed to by str. Returns 0 if the line given is in a valid format, 1
 * otherwise.
 *
 * @param	obj		Memory buffer to store the object properties
 * @param	type	The type of object to store
 * @param	str		The line to be parsed
 * @return	Whether the operation was successful
 */
int	save_object(t_object *object, int type, char *str)
{
	int		i;
	int		j;
	int		n;
	float	*mem;

	object->type = type;
	i = -1;
	while (++i < 7)
	{
		j = 3;
		while ((type & (1 << i)) && (--j >= 0))
		{
			n = ((1 << i) == A_NORM) + ((1 << i) == A_COL) * 2;
			mem = &object->vectors[n].e[2 - j];
			if ((1 << i) & (A_PCNT | A_FLT1 | A_FLT2 | A_FOV))
				mem = &object->param[i - 2];
			j -= j * (((A_PCNT | A_FLT1 | A_FLT2 | A_FOV) & (1 << i)) > 0);
			n = populate_buffer(str, mem, (1 << i));
			if (n == -1 || (j > 0 && str[n] != ','))
				return (1);
			str += n + (n > 0);
		}
	}
	return (0);
}

/**
 * Saves objects to t_scene struct by parsing the given string pointed to by
 * str, skipping first any leading whitespaces before parsing the line. Returns
 * 0 if the line parsed if valid, 1 otherwise.
 *
 * The following are valid object types:
 * 	AMBI	-	ambient light
 * 	CAMERA	-	camera
 * 	LIGHT	-	point light
 * 	SPHERE	-	sphere
 * 	PLANE	-	plane
 * 	CYLND	-	cylinder
 *
 * @param	scene	Memory buffer to store the objects and their corresponding
 * 					properties
 * @param	str		The line to be parsed
 * @return	Whether the operation was successful
 */
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
	if ((type & ((SPHERE | PLANE | CYLND) & ~0xff)) && scene->n_obj < MAX_OBJ)
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