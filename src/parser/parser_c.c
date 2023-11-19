/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_c.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/09 15:18:10 by rraharja          #+#    #+#             */
/*   Updated: 2023/11/19 10:21:04 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser_c.h"

int	n_atof(char *str, float *val)
{
	int		*n;
	int		sign;
	double	point;
	double	total;

	n = (int []){0, 0, 0};
	total = 0;
	while (ft_isspace(str[n[0]]) && str[n[0]] != '\n')
		n[0]++;
	sign = (str[n[0]] == '+') - (str[n[0]] == '-');
	n[0] += sign != 0;
	while (str[n[0] + n[1]] >= '0' && str[n[0] + n[1]] <= '9')
	{
		total *= 10;
		total += str[n[0] + n[1]++] - '0';
	}
	point = (str[n[0] + n[1] + n[2]] == '.' && ++n[0]);
	while (str[n[0] + n[1] + n[2]] >= '0' && str[n[0] + n[1] + n[2]] <= '9')
	{
		point /= 10;
		total += (str[n[0] + n[1] + n[2]++] - '0') * point;
	}
	*val = (sign + (sign == 0)) * total;
	return ((n[0] + n[1] + n[2]) * ((n[1] + n[2]) > 0));
}

int	bns_obj_type(char *str)
{
	if (*str == 'R' && ft_isspace(str[1]) && str[1] != '\n')
		return (SIZE);
	if (!ft_strncmp(str, "CB", 2) && ft_isspace(str[2]) && str[2] != '\n')
		return (QMAP);
	if (!ft_strncmp(str, "lp", 2) && ft_isspace(str[2]) && str[2] != '\n')
		return (PTLGHT);
	if (!ft_strncmp(str, "ls", 2) && ft_isspace(str[2]) && str[2] != '\n')
		return (SPLGHT);
	if (!ft_strncmp(str, "bx", 2) && ft_isspace(str[2]) && str[2] != '\n')
		return (BOX);
	if (!ft_strncmp(str, "cn", 2) && ft_isspace(str[2]) && str[2] != '\n')
		return (CONE);
	if (!ft_strncmp(str, "tr", 2) && ft_isspace(str[2]) && str[2] != '\n')
		return (TRIAGL);
	if (!ft_strncmp(str, "qd", 2) && ft_isspace(str[2]) && str[2] != '\n')
		return (QUAD);
	if (!ft_strncmp(str, "ds", 2) && ft_isspace(str[2]) && str[2] != '\n')
		return (DISK);
	return (INVAL);
}

int	obj_type(char *str)
{
	if (*str == 'A' && ft_isspace(str[1]) && str[1] != '\n')
		return (AMBI);
	if (*str == 'C' && ft_isspace(str[1]) && str[1] != '\n')
		return (CAMERA);
	if (!ft_strncmp(str, "sp", 2) && ft_isspace(str[2]) && str[2] != '\n')
		return (SPHERE);
	if (!ft_strncmp(str, "pl", 2) && ft_isspace(str[2]) && str[2] != '\n')
		return (PLANE);
	if (!ft_strncmp(str, "cy", 2) && ft_isspace(str[2]) && str[2] != '\n')
		return (CYLND);
	if (!BNS && *str == 'L' && ft_isspace(str[1]) && str[1] != '\n')
		return (PTLGHT);
	if (BNS)
		return (bns_obj_type(str));
	return (INVAL);
}

int	populate_buffer(char *str, float *mem, int obj_type, int i)
{
	int				n;
	int				type;
	static float	i_angle;

	type = (obj_type >> 3 * i) & 0x7;
	n = n_atof(str, mem);
	if ((type == NORM && (*mem < -1 || *mem > 1))
		|| (type == UIN8 && (*mem < 0 || *mem > 255))
		|| (type == PCNT && (*mem < 0 || *mem > 1))
		|| (type == ANGL && (*mem < 0 || *mem > 180))
		|| (type == OAGL && (*mem < i_angle || *mem > 180)))
		return (-1);
	if (obj_type == SPLGHT && type == ANGL)
		i_angle = *mem;
	if (type == UIN8)
		*mem /= 255.f;
	if (((obj_type != SIZE && obj_type != BOX) && type == FLTS)
		|| (obj_type == BOX && i == 3))
		*mem /= 2.f;
	return (n);
}
