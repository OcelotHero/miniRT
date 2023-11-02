/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/11 09:35:18 by rraharja          #+#    #+#             */
/*   Updated: 2023/11/02 08:39:08 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

# include "types_c.h"

# if BNS == 1
#  include "types_b.h"
# endif

# include "ft_dprintf.h"

# if BNS == 1

int	cleanup(t_rtx *rtx, int errcode);
# endif

int	error_msg(char *format, ...);

#endif