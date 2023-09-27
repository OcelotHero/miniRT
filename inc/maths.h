/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   maths.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/28 20:40:10 by rraharja          #+#    #+#             */
/*   Updated: 2023/09/26 13:00:53 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MATHS_H
# define MATHS_H

# include "types.h"

t_vec3	vec3_elem_op(t_vec3 a, char op, t_vec3 b);
t_vec3	vec3_scale(float a, t_vec3 v);
t_vec3	vec3_normalize(t_vec3 a);
float	vec3_dot(t_vec3 a, t_vec3 b);
float	vec3_length(t_vec3 a);

t_vec4	vec4_elem_op(t_vec4 a, char op, t_vec4 b);
t_vec4	vec4_scale(float a, t_vec4 v);
t_vec4	vec4_normalize(t_vec4 a);
float	vec4_dot(t_vec4 a, t_vec4 b);
float	vec4_length(t_vec4 a);

#endif
