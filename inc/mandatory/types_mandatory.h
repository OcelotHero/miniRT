/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types_mandatory.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 10:01:19 by rraharja          #+#    #+#             */
/*   Updated: 2023/10/28 10:01:19 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPES_MANDATORY_H
# define TYPES_MANDATORY_H

# define WIDTH	400

typedef struct s_object
{
	int		type;
	t_vec3	pos;
	t_vec3	axis;
	t_vec3	color;
	float	param[4];
}	t_object;

typedef struct s_scene
{
	int			n_obj;
	t_object	ambient;
	t_object	camera;
	t_object	light;
	t_object	objects[MAX_SIZE];
}	t_scene;

#endif