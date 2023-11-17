/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_b.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/11 09:35:18 by rraharja          #+#    #+#             */
/*   Updated: 2023/11/17 01:21:36 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_B_H
# define UTILS_B_H

# include "types_c.h"
# include "types_b.h"

void	set_int(uint32_t pg, char *id, int a);
void	set_float(uint32_t pg, char *id, float a);
void	set_float2(uint32_t pg, char *id, float a, float b);

#endif