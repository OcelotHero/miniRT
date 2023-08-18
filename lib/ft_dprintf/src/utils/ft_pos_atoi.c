/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_pos_atoi.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/23 14:59:16 by rraharja          #+#    #+#             */
/*   Updated: 2022/10/23 15:26:53 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_dprintf.h"

size_t	ft_pos_atoi(const char **str)
{
	size_t	total;

	total = 0;
	while (**str >= '0' && **str <= '9')
	{
		total *= 10;
		total += **str - '0';
		(*str)++;
	}
	return (total);
}
