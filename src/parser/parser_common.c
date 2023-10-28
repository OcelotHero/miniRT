/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_common.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/09 15:18:10 by rraharja          #+#    #+#             */
/*   Updated: 2023/10/28 10:45:26 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser_common.h"

int	n_atof(char *str, float *val)
{
	int		*i;
	int		sign;
	double	point;
	double	total;

	i = (int []){0, 0, 0};
	total = 0;
	while ((str[i[0]] >= '\t' && str[i[0]] <= '\r') || str[i[0]] == ' ')
		i[0]++;
	sign = (str[i[0]] == '+') - (str[i[0]] == '-');
	i[0] += sign != 0;
	while (str[i[0] + i[1]] >= '0' && str[i[0] + i[1]] <= '9')
	{
		total *= 10;
		total += str[i[0] + i[1]++] - '0';
	}
	point = (str[i[0] + i[1] + i[2]] == '.' && ++i[0]);
	while (str[i[0] + i[1] + i[2]] >= '0' && str[i[0] + i[1] + i[2]] <= '9')
	{
		point /= 10;
		total += (str[i[0] + i[1] + i[2]++] - '0') * point;
	}
	*val = (sign + (sign == 0)) * total;
	return ((i[0] + i[1] + i[2]) * ((i[1] + i[2]) > 0));
}

int	bns_obj_type(char *str)
{
	if ((*str == 'R') && (*(str + 1) == ' ' || *(str + 1) == '\t'))
		return (SIZE);
	if (!ft_strncmp(str, "CB", 2) && (*(str + 2) == ' ' || *(str + 2) == '\t'))
		return (QMAP);
	if (!ft_strncmp(str, "lp", 2) && (*(str + 2) == ' ' || *(str + 2) == '\t'))
		return (PTLGHT);
	if (!ft_strncmp(str, "ls", 2) && (*(str + 2) == ' ' || *(str + 2) == '\t'))
		return (SPLGHT);
	if (!ft_strncmp(str, "bx", 2) && (*(str + 2) == ' ' || *(str + 2) == '\t'))
		return (BOX);
	if (!ft_strncmp(str, "cn", 2) && (*(str + 2) == ' ' || *(str + 2) == '\t'))
		return (CONE);
	if (!ft_strncmp(str, "tr", 2) && (*(str + 2) == ' ' || *(str + 2) == '\t'))
		return (TRIAGL);
	if (!ft_strncmp(str, "qd", 2) && (*(str + 2) == ' ' || *(str + 2) == '\t'))
		return (QUAD);
	if (!ft_strncmp(str, "ds", 2) && (*(str + 2) == ' ' || *(str + 2) == '\t'))
		return (DISK);
	return (INVAL);
}

int	obj_type(char *str)
{
	if ((*str == 'A') && (*(str + 1) == ' ' || *(str + 1) == '\t'))
		return (AMBI);
	if ((*str == 'C') && (*(str + 1) == ' ' || *(str + 1) == '\t'))
		return (CAMERA);
	if (!ft_strncmp(str, "sp", 2) && (*(str + 2) == ' ' || *(str + 2) == '\t'))
		return (SPHERE);
	if (!ft_strncmp(str, "pl", 2) && (*(str + 2) == ' ' || *(str + 2) == '\t'))
		return (PLANE);
	if (!ft_strncmp(str, "cy", 2) && (*(str + 2) == ' ' || *(str + 2) == '\t'))
		return (CYLND);
	if (!BNS && (*str == 'L') && (*(str + 1) == ' ' || *(str + 1) == '\t'))
		return (PTLGHT);
	if (BNS)
		return (bns_obj_type(str));
	return (INVAL);
}

int	populate_buffer(char *str, float *mem, int obj_type, int type)
{
	int				n;
	static float	i_angle;

	n = n_atof(str, mem);
	if (((type == NORM) && (*mem < -1 || *mem > 1))
		|| ((type == UIN8) && (*mem < 0 || *mem > 255))
		|| ((type == PCNT) && (*mem < 0 || *mem > 1))
		|| ((type == ANGL) && (*mem < 0 || *mem > 180))
		|| ((type == OAGL) && (*mem < i_angle || *mem > 180)))
		return (-1);
	if (obj_type == SPLGHT && type == ANGL)
		i_angle = *mem;
	if (type == UIN8)
		*mem /= 255.f;
	if ((obj_type != SIZE || obj_type != BOX) && type == FLTS)
		*mem /= 2.f;
	return (n);
}
