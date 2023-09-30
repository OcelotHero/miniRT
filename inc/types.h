/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/11 09:35:18 by rraharja          #+#    #+#             */
/*   Updated: 2023/09/30 21:31:43 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPES_H
# define TYPES_H

# ifdef __APPLE__
#  define DENSITY 2.0f
# else
#  define DENSITY 1.0f
# endif

# define MV_SPEED		5.f * DENSITY
# define S_SENSITIVITY	1.f * DENSITY
# define M_SENSITIVITY	.01f * DENSITY

# define MAX_SIZE	256

# define A_POS	1 << 0
# define A_NORM	1 << 1
# define A_PCNT	1 << 2
# define A_FLT1	1 << 3
# define A_FLT2	1 << 4
# define A_FOV	1 << 5
# define A_COL	1 << 6

# define STB_IMAGE_IMPLEMENTATION
# define STB_IMAGE_RESIZE_IMPLEMENTATION

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

#  define WIDTH	400

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

#  include <stdlib.h>
#  include "MLX42/MLX42_Int.h"

#  define WIDTH		1600
#  define HEIGHT	900

typedef struct	s_material
{
	t_vec3	albedo;
	float	intensity;
	t_vec3	emissive;
	float	spec_roughness;
	t_vec3	spec_color;
	float	spec_chance;
	t_vec3	refr_color;
	float	refr_chance;
	t_vec3	texture;
	float	refr_roughness;
	t_vec4	normal_map;
	float	IOR;
	float	pad[3];
}	t_material;

typedef struct	s_object
{
	int			type;
	int			pad[3];
	t_vec3		pos;
	float		i_cone;
	t_vec3		axis;
	float		o_cone;
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

typedef struct	s_rtx
{
	bool	refresh;
	int		pos[2];
	int		size[2];
	float	yaw;
	float	pitch;
	float	start_time;
	float	delta_time;
	float	last_frame;
	GLuint	buf_a_program;
	GLuint	image_program;
	GLuint	framebuffer;
	GLuint	tex[16];
	t_scene	scene;
	mlx_t	*mlx;
}	t_rtx;

# endif

#endif