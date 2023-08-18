/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/13 20:06:12 by rraharja          #+#    #+#             */
/*   Updated: 2022/10/13 20:06:12 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_get_word_count(const char *str, char c)
{
	int	count;

	count = 0;
	while (*str)
	{
		while (*str && *str == c)
			str++;
		if (*str)
			count++;
		while (*str && *str != c)
			str++;
	}
	return (count);
}

static int	ft_get_word_length(const char *str, char c)
{
	int	length;

	length = 0;
	while (str[length] && str[length] != c)
		length++;
	return (length);
}

static char	*ft_duplicate_word(const char *str, int len)
{
	int		i;
	char	*buffer;

	buffer = malloc(sizeof(*buffer) * (len + 1));
	if (!buffer)
		return (NULL);
	i = 0;
	while (i < len)
	{
		buffer[i] = str[i];
		i++;
	}
	buffer[i] = '\0';
	return (buffer);
}

char	**ft_free_allocated(char **strs, size_t len)
{
	while (len--)
		free(strs[len]);
	free(strs);
	return (NULL);
}

char	**ft_split(char const *s, char c)
{
	int		i;
	int		count;
	int		length;
	char	**strs;

	if (!s)
		return (NULL);
	count = ft_get_word_count(s, c);
	strs = malloc(sizeof(*strs) * (count + 1));
	if (!strs)
		return (NULL);
	i = 0;
	while (i < count)
	{
		while (*s && *s == c)
			s++;
		length = ft_get_word_length(s, c);
		strs[i] = ft_duplicate_word(s, length);
		if (!strs[i])
			return (ft_free_allocated(strs, i));
		s += length;
		i++;
	}
	strs[i] = NULL;
	return (strs);
}
