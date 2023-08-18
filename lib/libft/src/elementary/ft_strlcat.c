/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 06:00:37 by rraharja          #+#    #+#             */
/*   Updated: 2022/10/12 08:23:41 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	size_t	length;

	length = 0;
	while (dst && dst[length] && length < dstsize)
		length++;
	if (dstsize > length)
	{
		while (length < (dstsize - 1) && *src)
		{
			dst[length] = *src;
			length++;
			src++;
		}
		dst[length] = '\0';
	}
	while (*src != '\0')
	{
		length++;
		src++;
	}
	return (length);
}
