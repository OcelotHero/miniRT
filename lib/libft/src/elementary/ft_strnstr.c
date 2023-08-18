/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 09:21:39 by rraharja          #+#    #+#             */
/*   Updated: 2022/10/12 09:26:53 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *haystack, const char *needle, size_t len)
{
	size_t	i;

	if (!needle[0])
		return ((char *) haystack);
	while (len && *haystack)
	{
		i = 0;
		while (i < len && needle[i] && haystack[i] == needle[i])
			i++;
		if (i == len && needle[i])
			return (NULL);
		if (!needle[i])
			return ((char *) haystack);
		len--;
		haystack++;
	}
	return (NULL);
}
