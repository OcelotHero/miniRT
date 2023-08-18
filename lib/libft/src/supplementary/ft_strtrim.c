/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/13 19:09:56 by rraharja          #+#    #+#             */
/*   Updated: 2022/10/13 19:09:56 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_constains(const char *str, const char c)
{
	while (*str && *str != c)
		str++;
	return (*str == c);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	int		i;
	int		last;
	char	*trimmed;

	if (!s1 || !set)
		return (NULL);
	while (*s1 && ft_constains(set, *s1))
		s1++;
	i = 0;
	last = -1;
	while (s1[i])
	{
		if (!ft_constains(set, s1[i]))
			last = i;
		i++;
	}
	trimmed = malloc(sizeof(*trimmed) * (last + 2));
	if (!trimmed)
		return (NULL);
	i = -1;
	while (++i <= last)
		trimmed[i] = s1[i];
	trimmed[last + 1] = '\0';
	return (trimmed);
}
