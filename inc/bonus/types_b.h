/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types_b.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 09:58:55 by rraharja          #+#    #+#             */
/*   Updated: 2023/10/28 09:58:55 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPES_B_H
# define TYPES_B_H

# include <stdlib.h>
# include "MLX42/MLX42_Int.h"

# define MV_SPEED		5.f
# define S_SENSITIVITY	1.f
# define M_SENSITIVITY	.01f

# define STB_IMAGE_IMPLEMENTATION
# define STB_IMAGE_RESIZE_IMPLEMENTATION

# define WIDTH	1600
# define HEIGHT	900

typedef struct s_material
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
	float	ior;
	float	pad[3];
}	t_material;

typedef struct s_object
{
	int			type;
	int			pad[3];
	t_vec3		pos;
	float		i_cone;
	t_vec3		axis;
	float		o_cone;
	float		param[4];
}	t_object;

typedef struct s_scene
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

typedef struct s_rtx
{
	bool	refresh;
	int		pos[2];
	int		size[2];
	float	yaw;
	float	pitch;
	float	start_time;
	float	delta_time;
	float	last_frame;
	float	cb_intensity;
	GLuint	buf_a_program;
	GLuint	image_program;
	GLuint	framebuffer;
	GLuint	tex[16];
	t_scene	scene;
	mlx_t	*mlx;
}	t_rtx;

#endif