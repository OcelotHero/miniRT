/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   intersections.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/29 18:27:24 by rraharja          #+#    #+#             */
/*   Updated: 2023/10/29 18:27:24 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "renderer.h"

bool	sp_hit(t_ray r, float *t_lim, t_record *rec, t_object *obj)
{
	t_vec3	m;
	float	b;
	float	c;
	float	t;
	float	dis;

	m = vec3_elem_op(r.ori, '-', obj->pos);
	b = vec3_dot(m, r.dir);
	c = vec3_dot(m, m) - obj->param[0] * obj->param[0];
	if (c > 0.0f && b > 0.0f)
		return (false);
	dis = b * b - c;
	if (dis < 0)
		return (false);
	t = -b - sqrt(dis);
	if (t <= t_lim[0] || t_lim[1] <= t)
		return (false);
	rec->t = t;
	rec->p = vec3_elem_op(r.ori, '+', vec3_scale(t, r.dir));
	rec->n = vec3_scale(1.f / obj->param[0],
			vec3_elem_op(m, '+', vec3_scale(t, r.dir)));
	rec->c = obj->color;
	return (true);
}

bool	pl_hit(t_ray r, float *t_lim, t_record *rec, t_object *obj)
{
	t_vec3	m;
	float	a;
	float	b;
	float	t;

	m = vec3_elem_op(r.ori, '-', obj->pos);
	a = vec3_dot(r.dir, obj->axis);
	b = vec3_dot(m, obj->axis);
	t = -b / a;
	if (t <= t_lim[0] || t > t_lim[1])
		return (false);
	rec->t = t;
	rec->p = vec3_elem_op(r.ori, '+', vec3_scale(t, r.dir));
	rec->n = vec3_scale((2 * (vec3_dot(r.dir, obj->axis) < 0)) - 1, obj->axis);
	rec->c = obj->color;
	return (true);
}

bool	cy_normal(float *in, t_ray r, t_record *rec, t_object *obj)
{
	t_vec3	n;

	n = vec3_normalize(obj->axis);
	if (in[0] < .5f)
	{
		rec->t = in[1];
		rec->p = vec3_elem_op(r.ori, '+', vec3_scale(in[1], r.dir));
		rec->n = vec3_scale(1.f / obj->param[0],
				vec3_elem_op(vec3_elem_op(rec->p, '-', obj->pos),
					'+', vec3_scale(-in[2], n)));
		rec->c = obj->color;
	}
	else
	{
		rec->t = in[1];
		rec->p = vec3_elem_op(r.ori, '+', vec3_scale(in[1], r.dir));
		rec->n = vec3_scale((in[2] > 0) - (in[2] < 0), n);
		rec->c = obj->color;
	}
	return (true);
}

bool	cy_hit(t_ray r, float *t_lim, t_record *rec, t_object *obj)
{
	t_vec3	n;
	t_vec3	m;
	float	*d;
	float	*a;
	float	res[3];

	n = vec3_normalize(obj->axis);
	m = vec3_elem_op(r.ori, '-', obj->pos);
	d = (float []){vec3_dot(n, r.dir), vec3_dot(n, m)};
	a = (float []){1.f - d[0] * d[0], vec3_dot(m, r.dir) - d[1] * d[0],
		vec3_dot(m, m) - d[1] * d[1] - obj->param[0] * obj->param[0]};
	res[0] = a[1] * a[1] - a[0] * a[2];
	if ((a[2] > 0.0f && a[1] > 0.0f) || res[0] < 0)
		return (false);
	res[0] = sqrt(res[0]);
	res[1] = (-a[1] - res[0]) / a[0];
	res[2] = d[1] + res[1] * d[0];
	if (res[1] > t_lim[0] && res[1] < t_lim[1]
		&& fabs(res[2]) <= obj->param[1])
		return (cy_normal((float []){0.f, res[1], res[2]}, r, rec, obj));
	res[1] = (((res[2] > 0) - (res[2] <= 0)) * obj->param[1] - d[1]) / d[0];
	if (res[1] > t_lim[0] && res[1] < t_lim[1]
		&& fabs(a[1] + a[0] * res[1]) <= res[0])
		return (cy_normal((float []){1.f, res[1], res[2]}, r, rec, obj));
	return (false);
}

bool	hit(t_ray r, float *t_lim, t_record *rec, t_scene *scene)
{
	int			i;
	bool		hit;
	float		min_t;
	t_object	*o;

	hit = false;
	min_t = t_lim[1];
	i = -1;
	while (++i < scene->n_obj)
	{
		o = &scene->objects[i];
		t_lim[1] = min_t;
		if ((o->type == (SPHERE >> 27 & 0x1f) && sp_hit(r, t_lim, rec, o))
			|| (o->type == (PLANE >> 27 & 0x1f) && pl_hit(r, t_lim, rec, o))
			|| (o->type == (CYLND >> 27 & 0x1f) && cy_hit(r, t_lim, rec, o)))
		{
			hit = true;
			min_t = rec->t;
		}
	}
	return (hit);
}
