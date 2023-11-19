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

# define E_USG	"To use run:\n\t./minirt_bonus <path_to_scene>\n"

// shader_program errors
# define E_VSC	"Vertex shader '%s' compilation failed!\n%s\n"
# define E_FSC	"Fragment shader '%s' compilation failed!\n%s\n"

# define E_FOP	"File open error: %s\n"
# define E_MLC	"Memory allocation error: %s\n"

# define E_SHL	"ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n"

// texture errors
# define E_TLD	"%s tex failed to load!\n"
# define E_TMX	"Maximum number of textures reached\n"

# define E_QGL	"Glob error: %s\n"
# define E_QNF	"%s cubemap tex not found!\n"
# define E_QLD	"%s cubemap tex failed to load!\n"
# define E_QWL	"Missing wildcard in filepath '%s'!\n"

// parser_b error
# define E_SDF	"Requested resolution invalid!\nSetting to default %dx%d\n"

// minirt_bonus errors
# define E_SHP	"Unable to create shader program\n"
# define E_FNC	"Framebuffer is not complete!\n"
# define E_BFO	"Unable to generate buffer objects\n"

# define I_SDF	"Resolution not specified\nSetting to default %dx%d\n"

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

typedef struct s_tex
{
	char	path[256];
	GLuint	id;
}	t_tex;

typedef struct s_rtx
{
	bool	refresh;
	int		pos[2];
	int		size[2];
	char	err[1024];
	float	yaw;
	float	pitch;
	float	start_time;
	float	delta_time;
	float	last_frame;
	float	cb_intensity;
	GLuint	buf_a_program;
	GLuint	image_program;
	GLuint	framebuffer;
	t_tex	tex[16];
	t_scene	scene;
	mlx_t	*mlx;
}	t_rtx;

#endif