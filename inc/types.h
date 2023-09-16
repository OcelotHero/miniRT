/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/11 09:35:18 by rraharja          #+#    #+#             */
/*   Updated: 2023/09/12 09:10:00 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPES_H
# define TYPES_H

# define MAX_SIZE	256

# define A_POS	1 << 0
# define A_NORM	1 << 1
# define A_PCNT	1 << 2
# define A_FLT1	1 << 3
# define A_FLT2	1 << 4
# define A_FOV	1 << 5
# define A_COL	1 << 6

enum e_token_types {
	INVAL	= 0x00000,
	AMBI	= 0x00100 | A_PCNT | A_COL,
	CAMERA	= 0x00200 | A_POS | A_NORM | A_FOV,
	LIGHT	= 0x00400 | A_POS | A_PCNT | A_COL,
	SPHERE	= 0x00800 | A_POS | A_FLT1 | A_COL,
	PLANE	= 0x01000 | A_POS | A_NORM | A_COL,
	CYLND	= 0x02000 | A_POS | A_NORM | A_FLT1 | A_FLT2 | A_COL
};

typedef union s_vec4 {
	struct {
		float	x;
		float	y;
		float	z;
		float	w;
	};
	struct {
		float	r;
		float	g;
		float	b;
		float	a;
	};
	float	e[4];
}	t_vec4;

typedef union s_vec3 {
	struct {
		float	x;
		float	y;
		float	z;
	};
	struct {
		float	r;
		float	g;
		float	b;
	};
	float	e[3];
}	t_vec3;

typedef struct	s_ray {
	t_vec3	ori;
	t_vec3	dir;
}	t_ray;

# ifndef BONUS
typedef struct	s_object
{
	int		type;
	t_vec3	pos;
	t_vec3	axis;
	t_vec3	color;
	float	param[4];
}	t_object;

typedef	struct	s_scene
{
	int			n_obj;
	t_object	ambient;
	t_object	camera;
	t_object	light;
	t_object	objects[MAX_SIZE];
}	t_scene;
# else
typedef struct	s_material
{
	t_vec4	albedo;
	t_vec4	emissive;
	t_vec4	specular_color;
	t_vec4	refraction_color;
	t_vec4	normal_map;
	float	intensity;
	float	specular_chance;
	float	specular_roughness;
	float	IOR;
	float	refraction_chance;
	float	refraction_roughness;
	float	pad[2];
}	t_material;

typedef struct	s_object
{
	int			type;
	int			pad[3];
	t_vec4		pos;
	t_vec4		axis;
	float		param[4];
}	t_object;

typedef	struct	s_scene
{
	int			n_obj;
	int			pad[3];
	t_object	camera;
	t_object	objects[MAX_SIZE];
	t_material	materials[MAX_SIZE];
	int			n_light;
	t_object	ambient;
	t_object	lights[MAX_SIZE];
}	t_scene;
# endif

#endif