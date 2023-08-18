/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_parse.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/23 15:09:10 by rraharja          #+#    #+#             */
/*   Updated: 2022/10/23 15:27:54 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_dprintf.h"

size_t	ft_parse(int fd, const char **format, va_list args)
{
	char		*start;
	t_pconfig	*config;

	if (!**format)
		return (0);
	start = (char *) *format;
	config = ft_parse_config(format);
	if (!config)
		return (0);
	if (!ft_contains(CONVERSION, config->conv))
		return (ft_dprint_none(fd, config, format, start));
	(*format)++;
	if (config->conv == '%')
		return (ft_dprint_percent(fd, config));
	if (config->conv == 'c')
		return (ft_dprint_char(fd, va_arg(args, int), config));
	if (config->conv == 's')
		return (ft_dprint_str(fd, va_arg(args, char *), ' ', config));
	if (config->conv == 'd' || config->conv == 'i')
		return (ft_dprint_nb_base(fd, va_arg(args, int), config));
	if (config->conv == 'p')
		return (ft_dprint_ptr(fd, va_arg(args, unsigned long long), config));
	return (ft_dprint_nb_base(fd, va_arg(args, unsigned int), config));
}
