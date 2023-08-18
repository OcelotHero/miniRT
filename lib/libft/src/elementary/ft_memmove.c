/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 08:38:13 by rraharja          #+#    #+#             */
/*   Updated: 2022/10/12 09:17:10 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	size_t	i;

	if (!dst && !src && len)
		return (NULL);
	i = -1;
	while (++i < len)
	{
		if (dst < src)
			((unsigned char *) dst)[i] = ((unsigned char *) src)[i];
		else
			((unsigned char *) dst)[len - i - 1]
				= ((unsigned char *) src)[len - i - 1];
	}
	return (dst);
}
