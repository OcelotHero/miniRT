/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_parse_config.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/23 15:01:38 by rraharja          #+#    #+#             */
/*   Updated: 2022/11/03 12:07:07 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_dprintf.h"

t_pconfig	*ft_parse_config(const char **format)
{
	t_pconfig	*config;

	config = ft_calloc(sizeof(*config), 1);
	if (!config)
		return (NULL);
	while (ft_contains(FLAG, *(++*format)))
	{
		if (**format == '#')
			config->form = 1;
		else if ((!config->padding || config->padding == '0')
			&& (**format == '0' || **format == '-'))
			config->padding = **format;
		else if ((!config->pos_prefix || config->pos_prefix == '+')
			&& (**format == ' ' || **format == '+'))
			config->pos_prefix = **format;
	}
	config->width = -1;
	if (ft_isdigit(**format))
		config->width = ft_pos_atoi(format);
	config->precision = -1;
	*format += **format == '.';
	if (*(*format - 1) == '.')
		config->precision = ft_pos_atoi(format);
	config->conv = **format;
	return (config);
}
