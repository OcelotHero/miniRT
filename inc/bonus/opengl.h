/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   opengl.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 07:16:28 by rraharja          #+#    #+#             */
/*   Updated: 2023/11/09 07:16:28 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OPENGL_H
# define OPENGL_H

# include "types_c.h"
# include "types_b.h"

# include <glob.h>
# include <errno.h>

int	setup_framebuffer(t_rtx *rtx);
int	setup_texture(t_rtx *rtx, char *path, int type);
int	setup_2d_texture(t_rtx *rtx, char *path);
int	setup_cubemap(t_rtx *rtx, char *dir);
int	setup_cubemap_texture(t_rtx *rtx, char *dir, int *prop, uint8_t **im);

#endif