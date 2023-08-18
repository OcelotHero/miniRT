/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 06:24:51 by rraharja          #+#    #+#             */
/*   Updated: 2022/10/12 07:36:27 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dst, const char *src, size_t dstsize)
{
	size_t	length;

	length = 0;
	if (dstsize != 0)
	{
		while (src[length] && length < (dstsize - 1))
		{
			dst[length] = src[length];
			length++;
		}
		dst[length] = '\0';
	}
	while (src[length])
		length++;
	return (length);
}
