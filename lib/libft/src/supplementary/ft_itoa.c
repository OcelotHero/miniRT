/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/13 21:47:12 by rraharja          #+#    #+#             */
/*   Updated: 2022/10/13 21:47:12 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_get_dest_length(long n)
{
	int	length;

	length = 0;
	if (n < 0)
	{
		n *= -1;
		length++;
	}
	while (n >= 10)
	{
		n /= 10;
		length++;
	}
	return (length + 1);
}

static void	ft_populate_buffer(long n, char *buffer)
{
	int	neg;

	neg = 0;
	if (n < 0)
	{
		n *= -1;
		neg = 1;
	}
	while (n >= 10)
	{
		*buffer = (n % 10) + '0';
		n /= 10;
		buffer--;
	}
	*buffer = n + '0';
	if (neg)
	{
		buffer--;
		*buffer = '-';
	}
}

char	*ft_itoa(int n)
{
	int		length;
	char	*result;

	length = ft_get_dest_length(n);
	result = malloc(sizeof(*result) * (length + 1));
	if (!result)
		return (NULL);
	ft_populate_buffer(n, &result[length - 1]);
	result[length] = '\0';
	return (result);
}
