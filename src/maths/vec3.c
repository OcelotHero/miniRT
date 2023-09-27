/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec3.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/10 19:28:57 by rraharja          #+#    #+#             */
/*   Updated: 2023/09/27 07:00:11 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "types.h"
#include <math.h>

t_vec3	vec3_elem_op(t_vec3 a, char op, t_vec3 b)
{
	int	i;

	if (op == 'x')
		return ((t_vec3){a.e[1] * b.e[2] - a.e[2] * b.e[1],
				a.e[2] * b.e[0] - a.e[0] * b.e[2],
				a.e[0] * b.e[1] - a.e[1] * b.e[0]});
	i = -1;
	while (++i < 3)
	{
		if (op == '+')
			a.e[i] += b.e[i];
		else if (op == '-')
			a.e[i] -= b.e[i];
		else if (op == '*')
			a.e[i] *= b.e[i];
		else if (op == '/')
			a.e[i] /= b.e[i];
	}
	return (a);
}

t_vec3	vec3_scale(float a, t_vec3 v)
{
	int	i;

	i = -1;
	while (++i < 3)
		v.e[i] *= a;
	return (v);
}

float	vec3_dot(t_vec3 a, t_vec3 b)
{
	int		i;
	float	res;

	i = -1;
	res = 0;
	while (++i < 3)
		res += a.e[i] * b.e[i];
	return (res);
}

float	vec3_length(t_vec3 a)
{
	int		i;
	float	res;

	i = -1;
	res = 0;
	while (++i < 3)
		res += a.e[i] * a.e[i];
	return (sqrt(res));
}

#include <stdio.h>

t_vec3	vec3_normalize(t_vec3 a)
{
	int			i;
	float		y;
	float		x2;
	const float threehalfs = 1.5f;

	y = vec3_dot(a, a);
	x2 = y * 0.5f;
	i  = *(int *) &y;
	i  = 0x5f3759df - (i >> 1);
	y  = *(float *) &i;
	y  = y * (threehalfs - (x2 * y * y));
	y  = y * (threehalfs - (x2 * y * y));
	return (vec3_scale(y, a));
}
