/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   renderer.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/29 19:00:24 by rraharja          #+#    #+#             */
/*   Updated: 2023/10/29 19:00:24 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RENDERER_H
# define RENDERER_H

# include "maths.h"
# include "types_c.h"
# include "types_m.h"

// # include "libft.h"
# include "MLX42/MLX42.h"

# include <math.h>
# include <stdlib.h>

bool	sp_hit(t_ray r, float *t_lim, t_record *rec, t_object *obj);
bool	pl_hit(t_ray r, float *t_lim, t_record *rec, t_object *obj);
bool	cy_hit(t_ray r, float *t_lim, t_record *rec, t_object *obj);
bool	cy_normal(float *in, t_ray r, t_record *rec, t_object *obj);
bool	hit(t_ray r, float *t_lim, t_record *rec, t_scene *scene);

t_vec3	r_color(t_ray r, t_scene *scene);
void	render(t_cam *cam, t_scene *scene, mlx_image_t *img);
void	set_px_color(mlx_image_t *img, t_cam *cam, int *pos, t_vec3 color);

#endif