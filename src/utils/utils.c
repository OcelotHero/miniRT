/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/26 22:29:08 by rraharja          #+#    #+#             */
/*   Updated: 2023/09/26 22:29:08 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"

#if BNS == 1

int	cleanup(t_rtx *rtx, int errcode)
{
	(void)rtx;
	return (1);
}
#endif

int	error_msg(char *format, ...)
{
	va_list	args;

	va_start(args, format);
	ft_vdprintf(2, format, args);
	va_end(args);
	return (1);
}
