/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strccmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 05:58:25 by rraharja          #+#    #+#             */
/*   Updated: 2022/10/12 08:23:31 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strccmp(const char *s1, const char *s2, int ignore_case)
{
	if (ignore_case)
		return (ft_strcmp_ign(s1, s2));
	return (ft_strcmp(s1, s2));
}
