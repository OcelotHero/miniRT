/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec4.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/10 19:28:57 by rraharja          #+#    #+#             */
/*   Updated: 2023/08/11 21:36:28 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "types.h"
#include <math.h>

t_vec4	vec4_elem_op(t_vec4 a, char op, t_vec4 b)
{
	int	i;

	i = -1;
	while (++i < 4)
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

t_vec4	vec4_scale(float a, t_vec4 v)
{
	int	i;

	i = -1;
	while (++i < 4)
		v.e[i] *= a;
	return (v);
}

float	vec4_dot(t_vec4 a, t_vec4 b)
{
	int		i;
	float	res;

	i = -1;
	res = 0;
	while (++i < 4)
		res += a.e[i] * b.e[i];
	return (res);
}

float	vec4_length(t_vec4 a)
{
	int		i;
	float	res;

	i = -1;
	res = 0;
	while (++i < 4)
		res += a.e[i] * a.e[i];
	return (sqrt(res));
}

t_vec4	vec4_normalize(t_vec4 a)
{
	long		i;
	float		y;
	float		x2;
	float		num;
	const float threehalfs = 1.5F;

	num = vec4_dot(a, a);
	x2 = num * 0.5F;
	y  = num;
	i  = *(long *) &y;
	i  = 0x5f3759df - (i >> 1);
	y  = *(float *) &i;
	y  = y * (threehalfs - (x2 * y * y));
	y  = y * (threehalfs - (x2 * y * y));
	return (vec4_scale(y, a));
}
