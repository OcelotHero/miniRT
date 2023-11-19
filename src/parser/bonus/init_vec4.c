
#include "parser_b.h"

static int wo_delim(cJSON *temp, t_json_data *data, t_vec4 *vec4, int default_val)
{
	float	val[3];
	char	*dup;
    char    *delim;
	int		index;

	dup = strdup(temp->valuestring);
    delim = strchr(dup, ':');
	*delim++ = '\0';
	vec4->w = get_path(dup, data);
	if (vec4->w == -2)
		return (free(dup), error_handler(data, temp, 0, "Invalid path"));
	if (n_atof(delim, &vec4->z) < strlen(delim))
		return (free(dup), error_handler(data, temp, delim - dup + 1, "Invalid path"));
	vec4->x = default_val;
	vec4->y = default_val;
	return (free(dup), 0);
}

static int w_delim(cJSON *temp, t_json_data *data, t_vec4 *vec4, int default_val)
{
	float	val[3];
	char	*dup;
    char    *delim;
	int		index;

	dup = strdup(temp->valuestring);
    delim = strchr(dup, ':');
	*delim = '\0';
	vec4->w = get_path(dup, data);
	if (vec4->w == -2)
		return (free(dup), error_handler(data, temp, 0, "Invalid path"));
	index = get_rgb(++delim, val);
	if (index == -1)
	{
		vec4->x = val[0];
		vec4->y = val[1];
		vec4->z = val[2];
		return (free(dup), 0);
	}
	else
		return (free(dup), error_handler(data, temp, strlen(dup) + index, "Invalid character"));
	return (free(dup), 0);
}

int init_vec4(cJSON *temp, t_json_data *data, t_vec4 *vec4, int default_val)
{
    char    *delim;

    if (strchr(temp->valuestring, ':') && ft_chcount(temp->valuestring, ',') == 2)
	{
		if (w_delim(temp, data, vec4, default_val))
			return (1);
	}
    else if (strchr(temp->valuestring, ':') && !ft_chcount(temp->valuestring, ','))
	{
		if (wo_delim(temp, data, vec4, default_val))
			return (1);
	}
	else
		return (error_handler(data, temp, 0, "Syntax error"));
    if (vec4->x > 1 || vec4->x < -1)
        return (error_handler(data, temp, delim - temp->valuestring + 2, "Invalid value"));
	if (vec4->y > 1 || vec4->y < -1)
        return (error_handler(data, temp, strchr(delim, ',') - temp->valuestring + 2, "Invalid value"));
	if (vec4->z < 0)
        return (error_handler(data, temp, strrchr(delim, ',') - temp->valuestring + 2, "Invalid value"));
	if (!vec4->w)
        return (error_handler(data, temp, 0, "Invalid value")); 
    return (0);
}
