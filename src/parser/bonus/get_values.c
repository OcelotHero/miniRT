
#include "parser_b.h"

int get_float(t_json_data *data, char *param, float *value, float default_value)
{
    cJSON    *temp;

    temp  = NULL;
    *value = default_value;
    temp = cJSON_GetObjectItemCaseSensitive(data->json[1], param);
    if (cJSON_IsNumber(temp))
        *value = (float) temp->valuedouble;
    if (cJSON_IsString(temp) && n_atof(temp->valuestring, value) < strlen(temp->valuestring))
        return (error_handler(data, temp, 0, "Invalid value"));
    if (temp && ((!strcmp(param, "intensity") && *value < 0) 
            || (!strcmp(param, "IOR") && *value < 1) 
            || (strcmp(param, "IOR") && strcmp(param, "intensity") 
                  && (*value < 0 || *value > 1))))
        return (error_handler(data, temp, 0, "Invalid value"));
    return (0);
}
 
int get_vec4(char *param, t_json_data *data, int default_val, t_vec4 *vec4)
{
	int format;
    cJSON    *temp;

    temp = cJSON_GetObjectItemCaseSensitive(data->json[1], param);
	if (!temp)
	{
		vec4->x = default_val;
		vec4->y = default_val;
		vec4->z = default_val;
		vec4->w = default_val;
	}
	else if (init_vec4(temp, data, vec4, 0))
		return (1);
    return (0);
}

int get_vec3(char *param, t_json_data *data, int default_val, t_vec3 *vec)
{
    cJSON    *temp;

    temp = cJSON_GetObjectItemCaseSensitive(data->json[1], param);
// 	if (temp)
// 	{
// // dprintf(2, "condition = %d\n", (strcmp("texture", temp->string) && init_vec3_path(temp, data, vec, default_val)));
// 		if (!strcmp("texture", temp->string) && init_vec3_path(temp, data, vec, default_val))
// 			return (1);
// 		else
// 		{
// 			if (init_vec3(temp, data, vec, default_val))
// 				return (1);
// 			*vec = vec3_scale(1/255.f, *vec);
// 		}
// 	}
// 	else
// 	{
		vec->x = default_val;
		vec->y = default_val;
		vec->z = default_val;
	// }
    return (0);
}

int get_rgb(char *str, float *val)
{
	int i;
	char *tmp;

	tmp = str;
	i = n_atof(tmp, &val[0]);
	if (!i || *(tmp + i) != ',')
		return (tmp - str);
	tmp = tmp + i + 1;
	i = n_atof(tmp, &val[1]);
	if (!i || *(tmp + i) != ',')
		return (tmp - tmp);
	tmp = tmp + i + 1;
	i = n_atof(tmp, &val[2]);
	if (!i)
		return (tmp - str);
	tmp = tmp + i;
	while (*tmp && ft_isspace(*tmp) && *tmp != '\n')
		tmp++;
	if (*tmp != '\0' && *tmp != '\n')
		return (tmp - str);
	return (-1);
}

int get_xy(char *str, float *val)
{
	int i;
	char *tmp;

	tmp = str;
	i = n_atof(tmp, &val[0]);
	if (!i || *(tmp + i) != ',')
		return (tmp - str);
	tmp = tmp + i + 1;
	i = n_atof(tmp, &val[1]);
	if (!i)
		return (tmp - str);
	tmp = tmp + i;
	while (*tmp && ft_isspace(*tmp) && *tmp != '\n')
		tmp++;
	if (*tmp != '\0' && *tmp != '\n')
		return (tmp - str);
	return (-1);
}

int get_path(char *path, t_json_data *data)
{
	int tmp;
	tmp = setup_texture(data->rtx, path, 0);
	if (!tmp)
	{
		if (!strcmp("checkered", path))
			return (-1);
		if (!strcmp("meta", path))
			return (1);
		return (-2);
	}
	return (tmp);
}
