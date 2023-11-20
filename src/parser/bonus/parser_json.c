
#include "parser_b.h"
#include <string.h>

static char	*exit_read(FILE *file)
{
	if (file != NULL)
		fclose(file);
	return (NULL);
}

static char	*read_file(const char *filename)
{
	FILE	*file;
	char	*content;
	long	longs[2];

	file = fopen(filename, "rb");
	if (file == NULL)
		return (exit_read(file));
	if (fseek(file, 0, SEEK_END) != 0)
		return (exit_read(file));
	longs[0] = ftell(file);
	if (longs[0] < 0)
		return (exit_read(file));
	if (fseek(file, 0, SEEK_SET) != 0)
		return (exit_read(file));
	content = (char *)malloc((size_t)longs[0] + sizeof(""));
	if (content == NULL)
		return (exit_read(file));
	longs[1] = fread(content, sizeof(char), (size_t)longs[0], file);
	if (longs[1] != longs[0])
		return (free(content), exit_read(file));
	content[longs[1]] = '\0';
	return (content);
}

int	init_obj(t_json_data *data, cJSON **mat_json, t_rtx *rtx, t_material *mat)
{
	if (get_float(data, "intensity", &mat->intensity, 1)
		|| get_float(data, "specularRoughness", &mat->spec_roughness, 0)
		|| get_float(data, "specularChance", &mat->spec_chance, 0)
		|| get_float(data, "refractionChance", &mat->refr_chance, 0)
		|| get_float(data, "refractionRoughness", &mat->refr_roughness, 0)
		|| get_float(data, "IOR", &mat->ior, 1))
		return (1);
	if (get_vec3("albedo", data, 1 * (cJSON_GetObjectItemCaseSensitive
				(mat_json[1], "texture") != 0), &mat->albedo)
		|| get_vec3("emissive", data, 0, &mat->emissive)
		|| get_vec3("specularColor", data, 1 * (cJSON_GetObjectItemCaseSensitive
				(mat_json[1], "texture") != 0), &mat->spec_color)
		|| get_vec3("refractionColor", data, 0, &mat->refr_color)
		|| get_vec3("texture", data, 0, &mat->texture)
		|| get_vec4("normalMap", data, 0, &mat->normal_map))
		return (1);
	return (0);
}

int	init_json(t_json_data *data, t_rtx *rtx, t_material *mat)
{
	cJSON	*material_json[2];
	int		status;

	material_json[0] = cJSON_Parse(data->line);
	if (!material_json[0])
		return (1);
	if (data->obj)
		material_json[1] = cJSON_GetObjectItem(material_json[0], data->obj);
	else
		material_json[1] = material_json[0];
	data->json = material_json;
	if (init_obj(data, material_json, rtx, mat))
	{
		free(data->line);
		cJSON_Delete(material_json[0]);
		return (1);
	}
	free(data->line);
	cJSON_Delete(material_json[0]);
	return (0);
}

int	parse_material_data(char *str, t_rtx *rtx, t_material *mat)
{
	char		*line[3];
	t_json_data	data;

	data.buf = rtx->err;
	data.rtx = rtx;
	data.file = str;
	data.obj = ft_strchr(str, ':');
	if (data.obj)
		*(data.obj++) = '\0';
	data.line = read_file(str);
	data.index = 0;
	if (init_json(&data, rtx, mat))
		return (1);
	return (0);
}
