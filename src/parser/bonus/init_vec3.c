
#include "parser_b.h"

int init_vec3_rgb(cJSON *temp, t_json_data *data, t_vec3 *vec3, float default_val)
{
	float	val[3];
    int     index;

    if (ft_chcount(temp->valuestring, ',') == 2)
	{
        index = get_rgb(temp->valuestring, val);
		if (index == -1)
        {
            vec3->r = val[0];
		    vec3->g = val[1];
		    vec3->b = val[2];
        }
        else
			return (error_handler(data, temp, index, "Invalid character"));
	}
    else
		return (error_handler(data, temp, 0, "Syntax error"));
    if (vec3->r > 255 || vec3->r < 0)
        return (error_handler(data, temp, 0, "Invalid value"));
	if (vec3->g > 255 || vec3->g < 0)
        return (error_handler(data, temp, strchr(temp->valuestring, ',') - temp->valuestring + 2, "Invalid value"));
	if (vec3->b > 255 || vec3->b < 0)
        return (error_handler(data, temp, strrchr(temp->valuestring, ',') - temp->valuestring + 2, "Invalid value"));
    return (0);
}

static int wo_delim(cJSON *temp, t_json_data *data, t_vec3 *vec3, int default_val)
{
	vec3->z = get_path(temp->valuestring, data);
	if (vec3->z == -2)
		return (error_handler(data, temp, 0, "Invalid path"));
	vec3->x = default_val;
	vec3->y = default_val;
	return (0);
}

static int w_delim(cJSON *temp, t_json_data *data, t_vec3 *vec3, int default_val)
{
	float	val[2];
	char	*dup;
    char    *delim;
	int		index;

	dup = strdup(temp->valuestring);
    delim = strchr(dup, ':');
	*delim = '\0';
	vec3->z = get_path(dup, data);
	if (vec3->z == -2)
		return (free(dup), error_handler(data, temp, 0, "Invalid path"));
	index = get_uv(++delim, val);
	if (index == -1)
	{
		vec3->x = val[0];
		vec3->y = val[1];
		return (free(dup), 0);
	}
	else
		return (free(dup), error_handler(data, temp, strlen(dup) + index, "Invalid character"));
	return (free(dup), 0);
}

int init_vec3_path(cJSON *temp, t_json_data *data, t_vec3 *vec3, float default_val)
{
    char    *delim;

    if (strchr(temp->valuestring, ':') && ft_chcount(temp->valuestring, ',') == 1)
	{
		if (w_delim(temp, data, vec3, default_val))
			return (1);
	}
    else if (!strchr(temp->valuestring, ':') && !ft_chcount(temp->valuestring, ','))
	{
		if (wo_delim(temp, data, vec3, default_val))
			return (1);
	}
	else
		return (error_handler(data, temp, 0, "Syntax error"));
    if (vec3->x < 0)
        return (error_handler(data, temp, delim - temp->valuestring + 2, "Invalid value"));
	if (vec3->y < 0)
        return (error_handler(data, temp, strrchr(delim, ',') - temp->valuestring + 2, "Invalid value"));
	if (!vec3->z)
        return (error_handler(data, temp, 0, "Invalid value")); 
    return (0);
}

// int init_vec3_path(cJSON *temp, t_json_data *data, t_vec3 *vec3, float default_val)
// {
// 	int		tex;
//     char    *delim;
// 	float	val[2];
// 	int		index;

//     delim = ft_strchr(temp->valuestring, ':');
//     if (!delim)
//     {
//         vec3->z = get_path(temp->valuestring, data);
// 		if (vec3->z == -2)
// 			return (error_handler(data, temp, 0, "Texture not found"));
//         vec3->x = default_val;
//         vec3->y = default_val;
//     }
//     else
//     {
//         *delim = '\0';
//         tex = get_path(temp->valuestring, data);
// 		if (tex == -2)
//         	return (error_handler(data, temp, 0, "Texture not found"));
// 		index = get_xy(temp->valuestring, val);
// 		if (index == -1)
// 		{
// 			vec3->x = val[0];
// 			vec3->y = val[1];
// 		}
// 		else
// 			return (error_handler(data, temp, index, "Syntax error"));
//     }
//     if (vec3->x > 1 || vec3->y > 1 || !vec3->z)
// 		return (1); // error message
//     return (0);
// }
