/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_bonus.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 10:08:37 by rraharja          #+#    #+#             */
/*   Updated: 2023/10/28 10:08:37 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_BONUS_H
# define PARSER_BONUS_H

#include "parser_common.h"
#include "types_bonus.h"

int	save_material(t_material *mat, int type, char *str);
int	save_object(t_object *obj, t_material *mat, int type, char *str);
int	save_objects(t_rtx *rtx, t_scene *scene, char *str);

#endif