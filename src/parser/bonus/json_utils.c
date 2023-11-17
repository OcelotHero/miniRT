
#include "parser_b.h"

// typedef struct {
//     int line;
//     int position;
// } Coordinates;

// Coordinates find_char_coordinates(char *json_content, char *obj_name, char *param, char target)
// {
//     Coordinates coordinates = { -1, -1 }; // Initialize with invalid coordinates

//     // Find the object name in the JSON content
//     const char *object_start = strstr(json_content, obj_name);
//     if (!object_start) {
//         printf("Object not found.\n");
//         return coordinates;
//     }

//     // Find the parameter within the object
//     const char *param_start = strstr(object_start, param);
//     if (!param_start) {
//         printf("Parameter not found.\n");
//         return coordinates;
//     }

//     // Find the target character within the parameter's value
//     const char *target_char = strchr(param_start, target);
//     if (!target_char) {
//         printf("Character not found.\n");
//         return coordinates;
//     }

//     // Calculate the line number (count newlines before the character)
//     const char *line_start = json_content;
//     while (line_start < target_char) {
//         if (*line_start == '\n') {
//             coordinates.line++;
//         }
//         line_start++;
//     }
//     coordinates.line++; // Start line count at 1

//     // Calculate the character's position within the line
//     coordinates.position = target_char - line_start + 1; // Start position count at 1

//     return coordinates;
// }

int error_handler(t_json_data *data, cJSON *line, int index, char *type)
{
	char *tmp;
    int i;

	tmp = data->buf + sprintf(data->buf, "\x1b[31mError: %s\x1b[0m\n%s\n|___"
                    , type, data->file);
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
    index += ft_strlen(line->string) + 3;
    while (i++ < index)
        tmp[i] = ' ';
    tmp = memcpy(tmp + i, "\x1b[32m^\n\x1b[0m", 14);
	return (1);
}

int ft_chcount(char *str, char ch)
{
	int i;
	int count;

	i = -1;
	count = 0;
	while (str[++i])
		if (str[i] == ch)
			count++;
	return (count);
}
