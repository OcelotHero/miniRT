/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strndup.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 09:58:28 by rraharja          #+#    #+#             */
/*   Updated: 2022/10/12 10:03:49 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strndup(const char *s1, size_t n)
{
	size_t	i;
	char	*dup;

	i = 0;
	while (i < n && s1[i])
		i++;
	n = i;
	dup = malloc(sizeof(*dup) * (n + 1));
	if (!dup)
		return (NULL);
	i = -1;
	while (++i < n)
		dup[i] = s1[i];
	dup[i] = '\0';
	return (dup);
}
