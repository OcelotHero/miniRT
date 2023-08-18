/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_fprintf.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/18 01:19:54 by rraharja          #+#    #+#             */
/*   Updated: 2022/10/18 01:19:54 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_dprintf.h"

int	ft_dprintf(int fd, const char *format, ...)
{
	int		i;
	int		total;
	va_list	args;

	va_start(args, format);
	total = 0;
	while (format && *format)
	{
		i = 0;
		while (format[i] && format[i] != '%')
			write(fd, &format[i++], 1);
		total += i;
		format += i;
		total += ft_parse(fd, &format, args);
	}
	va_end(args);
	return (total);
}

int	ft_vdprintf(int fd, const char *format, va_list args)
{
	int		i;
	int		total;

	total = 0;
	while (format && *format)
	{
		i = 0;
		while (format[i] && format[i] != '%')
			write(fd, &format[i++], 1);
		total += i;
		format += i;
		total += ft_parse(fd, &format, args);
	}
	return (total);
}
