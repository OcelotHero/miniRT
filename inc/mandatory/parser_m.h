/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_m.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 10:08:55 by rraharja          #+#    #+#             */
/*   Updated: 2023/10/28 10:08:55 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_M_H
# define PARSER_M_H

#include "parser_c.h"
#include "types_m.h"

int	save_object(t_object *obj, char *s, int *n);
int	save_objects(t_scene *scene, char *str);

#endif