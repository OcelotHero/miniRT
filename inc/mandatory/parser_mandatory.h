/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_mandatory.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 10:08:55 by rraharja          #+#    #+#             */
/*   Updated: 2023/10/28 10:08:55 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_MANDATORY_H
# define PARSER_MANDATORY_H

#include "parser_common.h"
#include "types_mandatory.h"

int	save_object(t_object *obj, int type, char *str);
int	save_objects(t_scene *scene, char *str);

#endif