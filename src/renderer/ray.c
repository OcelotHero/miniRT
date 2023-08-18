/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/10 18:46:15 by rraharja          #+#    #+#             */
/*   Updated: 2023/08/11 16:18:25 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "types.h"

t_vec3	vec3_elem_op(t_vec3 a, char op, t_vec3 b);
t_vec3	vec3_scale(float a, t_vec3 v);

t_vec3	ray_at(float t, t_ray ray)
{
	return (vec3_elem_op(ray.ori, '+', vec3_scale(t, ray.dir)));
}
