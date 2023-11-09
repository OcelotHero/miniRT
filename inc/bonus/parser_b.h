/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_b.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 10:08:37 by rraharja          #+#    #+#             */
/*   Updated: 2023/10/28 10:08:37 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_BONUS_H
# define PARSER_BONUS_H

# include "opengl.h"
# include "types_b.h"
# include "parser_c.h"

int	save_material(t_rtx *rtx, t_material *mat, char *s, int *n);
int	save_object(t_rtx *rtx, t_object *obj, char *s, int *n);
int	save_objects(t_rtx *rtx, t_scene *scene, char *str);

#endif