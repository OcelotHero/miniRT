
#include "parser_b.h"

#include <string.h>
// void print_material(const t_material *material, char *file, char *obj)
// {
//     if (1 && 1)
//     {
//         printf("____________________________________________________________________\n");
//         printf("\n>> %s : %s\n\n", file, obj);
        
//         // floats
//         printf("Intensity:                 %f\n", material->intensity);
//         printf("Specular Roughness:        %f\n", material->spec_roughness);
//         printf("Specular Chance:           %f\n", material->spec_chance);
//         printf("Refractive Chance:         %f\n", material->refr_chance);
//         printf("Refractive Roughness:      %f\n", material->refr_roughness);
//         printf("IOR:                       %f\n", material->ior);

//         // vec_3
//         printf("\n");
//         printf("Albedo:                   (%f, %f, %f)\n", material->albedo.r, material->albedo.g, material->albedo.b);
//         printf("Emissive:                 (%f, %f, %f)\n", material->emissive.r, material->emissive.g, material->emissive.b);
//         printf("Specular Color:           (%f, %f, %f)\n", material->spec_color.r, material->spec_color.g, material->spec_color.b);
//         printf("Refractive Color:         (%f, %f, %f)\n", material->refr_color.r, material->refr_color.g, material->refr_color.b);
//         printf("Texture:                  (%f, %f, %f)\n", material->texture.r, material->texture.g, material->texture.b);
        
//         // vec_4
//         printf("\n");
//         printf("Normal Map:               (%f, %f, %f, %f)\n", material->normal_map.x, material->normal_map.y, material->normal_map.z, material->normal_map.w);
        
//         // float[3]
//         printf("\n");
//         printf("Pad:                      [%f, %f, %f]\n", material->pad[0], material->pad[1], material->pad[2]);
//         printf("\n");
//     }
// }

static char *zbimbo(FILE *file)
{
    if (file != NULL)
        fclose(file);
    return (NULL);
}

static char *read_file(const char *filename)
{
    FILE *file;
    char *content;
    long longs[2];

    file = fopen(filename, "rb");
    if (file == NULL)
        return (zbimbo(file));
    if (fseek(file, 0, SEEK_END) != 0)
        return (zbimbo(file));
    longs[0] = ftell(file);
    if (longs[0] < 0)
        return (zbimbo(file));
    if (fseek(file, 0, SEEK_SET) != 0)
        return (zbimbo(file));
    content = (char*)malloc((size_t)longs[0] + sizeof(""));
    if (content == NULL)
        return (zbimbo(file));
    longs[1] = fread(content, sizeof(char), (size_t)longs[0], file);
    if (longs[1] != longs[0])
        return (free(content), zbimbo(file));
    content[longs[1]] = '\0';
    return (content);
}

int init_obj(t_json_data *data, cJSON **material_json, t_rtx *rtx, t_material *mat)
{
    if (get_float(data, "intensity", &mat->intensity, 1)
            || get_float(data, "specularRoughness", &mat->spec_roughness, 0)
            || get_float(data, "specularChance", &mat->spec_chance, 0)
            || get_float(data, "refractionChance", &mat->refr_chance, 0)
            || get_float(data, "refractionRoughness", &mat->refr_roughness, 0)
            || get_float(data, "IOR", &mat->ior, 1))
        return (1);
    if (get_vec3("albedo", data, 1 * (cJSON_GetObjectItemCaseSensitive(material_json[1], "texture") != 0), &mat->albedo)
            || get_vec3("emissive", data, 0, &mat->emissive)
            || get_vec3("specularColor", data, 1 * (cJSON_GetObjectItemCaseSensitive(material_json[1], "texture") != 0), &mat->spec_color)
            || get_vec3("refractionColor", data, 0, &mat->refr_color)
            || get_vec3("texture", data, 0, &mat->texture)
            || get_vec4("normalMap", data, 0, &mat->normal_map))
        return (1);
    return (0);
}

int init_json(t_json_data *data, t_rtx *rtx, t_material *mat)
{
    cJSON *material_json[2];
    int status;

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

int parse_material_data(char *str, t_rtx *rtx, t_material *mat)
{
    char        *line[3];
	t_json_data data;

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
	// print_material(mat, data.file, data.obj);
    return (0);
}
