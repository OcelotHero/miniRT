/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_parse_config.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/23 15:01:38 by rraharja          #+#    #+#             */
/*   Updated: 2022/10/23 15:45:40 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_dprintf.h"

t_pconfig	*ft_parse_config(const char **format)
{
	t_pconfig	*config;

	config = ft_calloc(sizeof(*config), 1);
	if (!config)
		return (NULL);
	config->width = -1;
	config->precision = -1;
	config->conv = *(++*format);
	return (config);
}
