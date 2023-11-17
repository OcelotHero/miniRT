/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types_m.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 10:01:19 by rraharja          #+#    #+#             */
/*   Updated: 2023/10/28 10:01:19 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPES_M_H
# define TYPES_M_H

# define WIDTH	800
# define HEIGHT	450

# define SAMPLE 25

# define E_USG	"To use run:\n\t./minirt <path_to_scene>\n"

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

typedef struct s_record
{
	float	t;
	t_vec3	p;
	t_vec3	n;
	t_vec3	c;
}	t_record;

typedef struct s_cam
{
	int		i_width;
	int		i_height;
	int		samples;
	t_vec3	vecs[3];
	t_vec3	center;
	t_vec3	p00_loc;
	t_vec3	delta_u;
	t_vec3	delta_v;
}	t_cam;

#endif