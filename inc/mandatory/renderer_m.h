/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   renderer_m.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/29 19:00:24 by rraharja          #+#    #+#             */
/*   Updated: 2023/10/29 19:00:24 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RENDERER_M_H
# define RENDERER_M_H

# include "maths.h"
# include "types_c.h"
# include "types_m.h"

# include "MLX42/MLX42.h"

# include <math.h>
# include <stdlib.h>

bool	hit(t_ray r, float *t_lim, t_record *rec, t_scene *scene);

void	render(t_cam *cam, t_scene *scene, mlx_image_t *img);

#endif