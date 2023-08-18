/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_dprint_null.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/23 15:06:33 by rraharja          #+#    #+#             */
/*   Updated: 2022/10/24 20:25:35 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_dprintf.h"

size_t	ft_dprint_null(int fd, t_pconfig *config, bool is_ptr)
{
	if (is_ptr)
		return (ft_dprint_str(fd, "0x0", ' ', config));
	return (ft_dprint_str(fd, "(null)", ' ', config));
}
