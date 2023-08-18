/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcmp_ign.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 05:58:25 by rraharja          #+#    #+#             */
/*   Updated: 2022/10/12 08:23:31 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strcmp_ign(const char *s1, const char *s2)
{
	size_t	i;

	i = 0;
	while (s1[i] && s2[i] && (s1[i] == s2[i]
			|| (ft_isalpha(s1[i]) && (s1[i] + 'a' - 'A') == s2[i])
			|| (ft_isalpha(s2[i]) && s1[i] == (s2[i] + 'a' - 'A'))))
		i++;
	return ((unsigned char) s1[i] - s2[i]);
}
