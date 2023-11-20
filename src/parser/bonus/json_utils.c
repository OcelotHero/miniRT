
#include "parser_b.h"

int	error_handler(t_json_data *data, cJSON *line, int index, char *type)
{
	char	*tmp;
	int		i;

	tmp = data->buf + sprintf(data->buf, "\x1b[31mError: %s\x1b[0m\n%s\n|___",
			type, data->file);
	i = 4;
	if (data->obj)
	{
		tmp += sprintf(tmp, "%s\n    |___", data->obj);
		i = 0;
	}
	tmp += sprintf(tmp, "%s : ", line->string);
	if (cJSON_IsString(line))
		tmp += sprintf(tmp, "\"%s\"\n        ", line->valuestring) - i;
	else if (cJSON_IsNumber(line))
		tmp += sprintf(tmp, "%f\n        ", line->valuedouble) - 1 - i;
	i = -1;
	index += strlen(line->string) + 3;
	while (i++ < index)
		tmp[i] = ' ';
	tmp = memcpy(tmp + i, "\x1b[32m^\n\x1b[0m", 14);
	return (1);
}

int	ft_chcount(char *str, char ch)
{
	int	i;
	int	count;

	i = -1;
	count = 0;
	while (str[++i])
		if (str[i] == ch)
			count++;
	return (count);
}

int	get_rgb(char *str, float *val)
{
	int		i;
	char	*tmp;

	tmp = str;
	i = n_atof(tmp, &val[0]);
	if (!i || *(tmp + i) != ',')
		return (tmp - str);
	tmp = tmp + i + 1;
	i = n_atof(tmp, &val[1]);
	if (!i || *(tmp + i) != ',')
		return (tmp - str);
	tmp = tmp + i + 1;
	i = n_atof(tmp, &val[2]);
	if (!i)
		return (tmp - str);
	tmp = tmp + i;
	while (*tmp && isspace(*tmp) && *tmp != '\n')
		tmp++;
	if (*tmp != '\0' && *tmp != '\n')
		return (tmp - str);
	return (-1);
}

int	get_uv(char *str, float *val)
{
	int		i;
	char	*tmp;

	tmp = str;
	i = n_atof(tmp, &val[0]);
	if (!i || *(tmp + i) != ',')
		return (tmp - str);
	tmp = tmp + i + 1;
	i = n_atof(tmp, &val[1]);
	if (!i)
		return (tmp - str);
	tmp = tmp + i;
	while (*tmp && isspace(*tmp) && *tmp != '\n')
		tmp++;
	if (*tmp != '\0' && *tmp != '\n')
		return (tmp - str);
	return (-1);
}

int	get_path(char *path, t_json_data *data)
{
	int	tmp;

	if (!strcmp("checkered", path))
		return (-1);
	if (!strcmp("meta", path))
		return (1);
	tmp = setup_texture(data->rtx, path, 0);
	if (tmp)
		return (tmp);
	return (-2);
}
