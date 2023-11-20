/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_b.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 09:52:43 by rraharja          #+#    #+#             */
/*   Updated: 2023/10/28 09:52:43 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser_b.h"

static int	parse_path(t_rtx *rtx, t_material *mat, char *s, int *n)
{
	int		r;
	char	c;
	char	*sep;

	sep = &s[*n];
	if (*sep == '"')
		sep = ft_strchr(&s[*n + 1], '"');
	else
		while (*sep && !isspace(*sep))
			sep++;
	if (!sep)
		return (0);
	c = *sep;
	*sep = '\0';
	if (n[1] == QMAP)
	{
		r = setup_texture(rtx, &s[*n + (s[*n] == '"')], n[1]);
		*sep = c;
		return ((sep - &s[*n] + (c == '"')) * (r != 0));
	}
	r = parse_material_data(&s[*n + (s[*n] == '"')], rtx, mat);
	*sep = c;
	return ((sep - &s[*n] + (c == '"')) * !r);
}

static int	save_material(t_rtx *rtx, t_material *mat, char *s, int *n)
{
	int	i;
	int	l;
	int	m;

	i = 3;
	m = *n;
	while (n[1] != QMAP && --i >= 0)
	{
		l = populate_buffer(&s[m], &mat->albedo.e[2 - i], n[1], 0);
		if (l <= 0 || (i && s[m + l] != ','))
			break ;
		m += l + (i && s[m + l] == ',');
	}
	if (n[1] == QMAP || i > -1)
		m = *n + parse_path(rtx, mat, s, n);
	else
		mat->ior = 1;
	while (isspace(s[m]))
		m++;
	*n = m;
	return (s[*n] != '\0');
}

static int	save_object(t_rtx *rtx, t_object *obj, char *s, int *n)
{
	int	i;
	int	j;
	int	l;

	obj->type = n[1] >> 27 & 0x1f;
	i = 9;
	while (--i > 0 || (!i && n[1] < SPHERE && n[1] != QMAP))
	{
		j = 3;
		while (((n[1] >> 3 * i) & 0x7) && --j >= 0)
		{
			l = populate_buffer(&s[*n], (float *)&obj->pos + (2 - j) + (3 - i)
					* (i < 4 && i) + ((int)fmax(i, 5) % 2 + (i < 5)) * 4
					+ (i > 3 && i < 7) * 3, n[1], i);
			j -= j * (((n[1] >> 3 * i) & 0x7) > NORM);
			if (l <= 0 || (j && s[*n + l] != ',')
				|| (i == 7 && !j && vec3_length(obj->axis) == 0.f))
				return (*n);
			*n += l + (j && s[*n + l] == ',');
		}
	}
	while (isspace(s[*n]) && (i || !i && s[*n] != '\n'))
		(*n)++;
	return (((i && s[*n]) || (!i && save_material(rtx, &rtx->scene.materials[
						(int)fmax(rtx->scene.n_obj - 1, 0)], s, n))) * *n);
}

static int	save_config(t_rtx *rtx, char *s, int n, int type)
{
	int			r;
	t_object	o;

	if (type == SIZE && !rtx->size[0] && !rtx->size[1]
		&& !save_object(rtx, &o, s, (int []){n, type}))
	{
		if (o.param[0] < 100 || o.param[0] > 3500
			|| o.param[1] < 100 || o.param[1] > 3500)
		{
			snprintf(rtx->err, sizeof(rtx->err), E_SDF, WIDTH, HEIGHT);
			ft_memcpy(o.param, (float []){WIDTH, HEIGHT}, 2 * sizeof(float));
		}
		ft_memcpy(rtx->size, (int []){o.param[0], o.param[1]}, 2 * sizeof(int));
		return (0);
	}
	if (type == QMAP && !rtx->tex[1].id)
	{
		r = save_object(rtx, &o, s, (int []){n, type});
		rtx->cb_intensity = o.param[3];
		return (r);
	}
	return (-(s[n] && s[n] != '\n'));
}

int	save_objects(t_rtx *rtx, t_scene *scene, char *s)
{
	int	n;
	int	type;

	n = 0;
	while (isspace(s[n]) && s[n] != '\n')
		n++;
	if (s[n] == '#')
		return (0);
	type = obj_type(&s[n]);
	n += (type != 0) + (type == QMAP || type >= PTLGHT);
	if (type == AMBI && !scene->ambient.type)
		return (save_object(rtx, &scene->ambient, s, (int []){n, type}));
	if (type == CAMERA && !scene->camera.type)
		return (save_object(rtx, &scene->camera, s, (int []){n, type}));
	if ((type == PTLGHT || type == SPLGHT) && scene->n_light < MAX_SIZE)
	{
		scene->lights[scene->n_light] = (t_object){.i_cone = 180.f,
			.o_cone = 180.f};
		return (save_object(rtx, &scene->lights[scene->n_light++], s,
				(int []){n, type}));
	}
	if (type >= SPHERE && scene->n_obj < MAX_SIZE)
		return (save_object(rtx, &scene->objects[scene->n_obj++], s,
				(int []){n, type}));
	return (save_config(rtx, s, n, type));
}
