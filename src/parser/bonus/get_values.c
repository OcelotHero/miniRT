
#include "parser_b.h"

int	get_float(t_json_data *data, char *param, float *value, float default_value)
{
	cJSON	*temp;

	temp = NULL;
	*value = default_value;
	temp = cJSON_GetObjectItemCaseSensitive(data->json[1], param);
	if (cJSON_IsNumber(temp))
		*value = (float) temp->valuedouble;
	if (cJSON_IsString(temp) && n_atof(temp->valuestring, value)
		< strlen(temp->valuestring))
		return (error_handler(data, temp, 0, "Invalid value"));
	if (temp && ((!strcmp(param, "intensity") && *value < 0)
			|| (!strcmp(param, "IOR") && *value < 1)
			|| (strcmp(param, "IOR") && strcmp(param, "intensity")
				&& (*value < 0 || *value > 1))))
		return (error_handler(data, temp, 0, "Invalid value"));
	return (0);
}

int	get_vec4(char *param, t_json_data *data, int default_val, t_vec4 *vec4)
{
	int		format;
	cJSON	*temp;

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

int	get_vec3(char *param, t_json_data *data, int default_val, t_vec3 *vec3)
{
	cJSON	*temp;

	temp = cJSON_GetObjectItemCaseSensitive(data->json[1], param);
	if (temp)
	{
		if (!strcmp("texture", temp->string))
		{
			if (init_vec3_path(temp, data, vec3, default_val))
				return (1);
		}
		else
		{
			if (init_vec3_rgb(temp, data, vec3, default_val))
				return (1);
			*vec3 = vec3_scale(1 / 255.f, *vec3);
		}
	}
	else
	{
		vec3->x = default_val;
		vec3->y = default_val;
		vec3->z = default_val;
	}
	return (0);
}
