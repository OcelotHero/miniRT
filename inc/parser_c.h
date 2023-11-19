/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_c.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/11 09:35:18 by rraharja          #+#    #+#             */
/*   Updated: 2023/11/19 10:20:53 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_C_H
# define PARSER_C_H

# include "libft.h"

# include "maths.h"
# include "types_c.h"

# include <math.h>

int	obj_type(char *str);
int	bns_obj_type(char *str);
int	n_atof(char *str, float *val);
int	populate_buffer(char *str, float *mem, int obj_type, int i);

#endif