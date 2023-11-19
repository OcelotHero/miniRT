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
# include "cJSON.h"

typedef	struct	s_json_data
{
	char	*buf;
	char	*file;
	char	*obj;
	char	*line;
	int		index;
	t_rtx	*rtx;
	cJSON	**json;
}	t_json_data;
int	save_material(t_rtx *rtx, t_material *mat, char *s, int *n);
int	save_object(t_rtx *rtx, t_object *obj, char *s, int *n);
int	save_objects(t_rtx *rtx, t_scene *scene, char *str);

int parse_material_data(char *str, t_rtx *rtx, t_material *mat);

int get_float(t_json_data *data, char *param, float *value, float default_value);
int get_vec4(char *param, t_json_data *data, int default_val, t_vec4 *vec);
int get_vec3(char *param, t_json_data *data, int default_val, t_vec3 *vec);
int get_rgb(char *str, float *val);
int get_uv(char *str, float *val);
int get_path(char *path, t_json_data *data);

int init_vec3_rgb(cJSON *temp, t_json_data *data, t_vec3 *vec3, float default_val);
int init_vec3_path(cJSON *temp, t_json_data *data, t_vec3 *vec3, float default_val);

int init_vec4(cJSON *temp, t_json_data *data, t_vec4 *vec4, int default_val);

int error_handler(t_json_data *data, cJSON *line, int index, char *type);
int ft_chcount(char *str, char ch);


#endif