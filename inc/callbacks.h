/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   callbacks.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/04 02:22:37 by rraharja          #+#    #+#             */
/*   Updated: 2023/03/04 02:22:37 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CALLBACKS_H
# define CALLBACKS_H

# include <math.h>

# include "maths.h"
# include "types.h"
# include "utils.h"

void	key_hook(mlx_key_data_t keydata, void *param);
void	cursor_hook(double xpos, double ypos, void *param);
void	scroll_hook(double xdelta, double ydelta, void *param);
void	mouse_hook(mouse_key_t button, action_t action,
			modifier_key_t mods, void *param);
void	loop_hook(void *param);

#endif
