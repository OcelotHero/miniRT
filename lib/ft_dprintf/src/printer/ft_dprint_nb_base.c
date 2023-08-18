/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_dprint_nb_base.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/23 15:04:15 by rraharja          #+#    #+#             */
/*   Updated: 2022/10/24 20:27:10 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_dprintf.h"

static size_t	ft_get_dest_length(long long n, t_pconfig *config)
{
	int					length;
	unsigned int		base;
	unsigned long long	nb;

	if (n == 0 && config->precision == 0)
		return (0);
	if (config->conv == 'd' || config->conv == 'i')
		nb = (1 - 2 * (n < 0)) * n;
	else
		nb = n;
	base = 10 + (config->conv == 'x' || config->conv == 'X') * 6;
	length = 0;
	while (nb >= base)
	{
		nb /= base;
		length++;
	}
	return (length + 1);
}

static void	ft_populate_buffer(long long n, char *buffer, int size,
								t_pconfig *config)
{
	int					neg;
	unsigned int		base;
	unsigned long long	nb;

	neg = (n < 0);
	if (config->conv == 'd' || config->conv == 'i')
		nb = (1 - 2 * neg) * n;
	else
		nb = n;
	base = 10 + (config->conv == 'x' || config->conv == 'X') * 6;
	while (nb >= base)
	{
		buffer[--size] = (BASE[nb % base] - ('x' - 'X')
				* (config->conv == 'X' && nb % base > 9));
		nb /= base;
	}
	buffer[--size] = BASE[nb] - (('x' - 'X') * (config->conv == 'X' && nb > 9));
	while (--size >= 0)
		buffer[size] = '0';
	if (neg && (config->conv == 'd' || config->conv == 'i'))
		buffer[0] = '-';
	else if (config->pos_prefix && (config->conv == 'd' || config->conv == 'i'))
		buffer[0] = config->pos_prefix;
	else if (n && (config->conv == 'x' || config->conv == 'X') && config->form)
		buffer[1] = config->conv;
}

static char	*ft_itoa_base(long long n, t_pconfig *config)
{
	int		length;
	char	*result;

	length = ft_get_dest_length(n, config);
	if (length < config->precision)
		length = config->precision;
	length += ((n < 0 || config->pos_prefix)
			&& (config->conv == 'd' || config->conv == 'i'));
	length += (length && n && ((config->conv == 'x' || config->conv == 'X')
				&& config->form)) * 2;
	if (config->padding == '0' && length < config->width)
		length = config->width;
	config->precision = length;
	result = malloc(sizeof(*result) * (length + 1));
	if (!result)
		return (NULL);
	result[length] = '\0';
	if (length)
		ft_populate_buffer(n, result, length, config);
	return (result);
}

size_t	ft_dprint_nb_base(int fd, long long n, t_pconfig *config)
{
	int		len;
	char	*str;
	char	padding;

	if (config->precision != -1 && config->padding == '0')
		config->padding = 0;
	str = ft_itoa_base(n, config);
	if (!str)
	{
		free(config);
		return (0);
	}
	padding = (config->padding == '0') * '0' + (config->padding != '0') * ' ';
	len = ft_dprint_str(fd, str, padding, config);
	free(str);
	return (len);
}
