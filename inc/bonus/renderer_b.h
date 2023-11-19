/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   renderer_b.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 07:16:28 by rraharja          #+#    #+#             */
/*   Updated: 2023/11/09 07:16:28 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RENDERER_B_H
# define RENDERER_B_H

# include "MLX42/MLX42_Int.h"

# include "utils.h"
# include "utils_b.h"
# include "types_c.h"
# include "types_b.h"

# include <glob.h>
# include <errno.h>

int			load_texture(t_rtx *rtx);
int			setup_texture(t_rtx *rtx, char *path, int type);

int			setup_framebuffer(t_rtx *rtx);
int			setup_buffer_objects(t_rtx *rtx);
int			setup_shader_program(t_rtx *rtx);
int			load_scene_to_buffer(t_rtx *rtx);

uint32_t	create_shader_program(const char *vert, const char *frag);

#endif