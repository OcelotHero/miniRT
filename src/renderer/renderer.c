/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   renderer.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/10 12:46:13 by rraharja          #+#    #+#             */
/*   Updated: 2023/10/28 10:22:01 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "maths.h"
#include "types_common.h"
#include "types_mandatory.h"
#include "parser_common.h"
#include "parser_mandatory.h"

#include "libft.h"
#include "MLX42/MLX42.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

t_vec3	ray_at(float t, t_ray ray);

typedef struct s_hit_record
{
	float	t;
	t_vec3	p;
	t_vec3	n;
	t_vec3	c;
}	t_hit_record;

typedef struct s_cam
{
	float	aspect;
	int		i_width;
	int		i_height;
	int		samples;
	int		max_depth;
	t_vec3	vecs[3];
	t_vec3	center;
	t_vec3	p00_loc;
	t_vec3	delta_u;
	t_vec3	delta_v;
}	t_cam;

bool	inter_surrounds(float *inter, float x)
{
	return (inter[0] < x && x < inter[1]);
}

bool	inter_contains(float *inter, float x)
{
	return (inter[0] <= x && x <= inter[1]);
}

float	inter_clamp(float *inter, float x)
{
	if (x < inter[0])
		return (inter[0]);
	if (x > inter[1])
		return (inter[1]);
	return (x);
}

t_vec3	inter_vec3_clamp(float *inter, t_vec3 v)
{
	int	i;

	i = -1;
	while (++i < 3)
	{
		if (v.e[i] < inter[0])
			v.e[i] = inter[0];
		else if (v.e[i] > inter[1])
			v.e[i] = inter[1];
	}
	return (v);
}

float	to_gamma(float linear)
{
	return (sqrt(linear));
}

float	randf(void)
{
	return (rand() / (RAND_MAX + 1.0f));
}

float	randfi(float min, float max)
{
	return (min + (max - min) * randf());
}

t_vec3	random_unit_vector(void)
{
	float	th;
	float	ph;

	th = randfi(-M_PI, M_PI);
	ph = randfi(-M_PI_2, M_PI_2);
	return ((t_vec3){cos(ph) * cos(th), cos(ph) * sin(th), sin(ph)});
}

bool	sp_hit(t_ray r, float *t_lim, t_hit_record *rec, t_object *obj)
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
	rec->p = ray_at(t, r);
	rec->n = vec3_scale(1.f / obj->param[0],
			vec3_elem_op(m, '+', vec3_scale(t, r.dir)));
	rec->c = obj->color;
	return (true);
}

bool	pl_hit(t_ray r, float *t_lim, t_hit_record *rec, t_object *obj)
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
	rec->p = ray_at(t, r);
	rec->n = vec3_scale((2 * (vec3_dot(r.dir, obj->axis) < 0)) - 1, obj->axis);
	rec->c = obj->color;
	return (true);
}

bool	cy_normal(float *in, t_ray r, t_hit_record *rec, t_object *obj)
{
	t_vec3	n;

	n = vec3_normalize(obj->axis);
	if (in[0] < .5f)
	{
		rec->t = in[1];
		rec->p = ray_at(in[1], r);
		rec->n = vec3_scale(1.f / obj->param[0],
				vec3_elem_op(vec3_elem_op(rec->p, '-', obj->pos),
					'+', vec3_scale(-in[2], n)));
		rec->c = obj->color;
	}
	else
	{
		rec->t = in[1];
		rec->p = ray_at(in[1], r);
		rec->n = vec3_scale((in[2] > 0) - (in[2] < 0), n);
		rec->c = obj->color;
	}
	return (true);
}

bool	cy_hit(t_ray r, float *t_lim, t_hit_record *rec, t_object *obj)
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

bool	obj_hit(t_ray r, float *t_lim, t_hit_record *rec, t_object *obj)
{
	if (obj->type == (SPHERE >> 27 & 0x1f))
		return (sp_hit(r, t_lim, rec, obj));
	if (obj->type == (PLANE >> 27 & 0x1f))
		return (pl_hit(r, t_lim, rec, obj));
	if (obj->type == (CYLND >> 27 & 0x1f))
		return (cy_hit(r, t_lim, rec, obj));
	return (false);
}

bool	hit(t_ray r, float *t_lim, t_hit_record *rec, t_scene *scene)
{
	int				i;
	bool			hit;
	float			min_t;
	t_hit_record	t_rec;

	hit = false;
	min_t = t_lim[1];
	i = -1;
	while (++i < scene->n_obj)
	{
		if (obj_hit(r, (float []){t_lim[0], min_t}, &t_rec, &scene->objects[i]))
		{
			hit = true;
			min_t = t_rec.t;
			*rec = t_rec;
		}
	}
	return (hit);
}

t_vec3 r_color(t_ray r, int depth, t_scene *scene)
{
	t_hit_record	rec;

	if (depth <= 0)
		return ((t_vec3){0.f, 0.f, 0.f});

	if (hit(r, (float []){.001f, 1.f / 0.f}, &rec, scene)) {
		t_vec3	l_dir = vec3_normalize(vec3_elem_op(scene->light.pos, '-', rec.p));
		float	dif = scene->light.param[3] * inter_clamp((float []){0.f, 1.f}, vec3_dot(rec.n, l_dir));
		t_vec3	diff = vec3_scale(dif, scene->light.color);

		t_vec3	ambi = vec3_scale(scene->ambient.param[3], scene->ambient.color);

		t_ray	n_r = (t_ray){rec.p, l_dir};
		t_hit_record	n_rec;
		if (hit(n_r, (float []){.001f, 1.f / 0.f}, &n_rec, scene)
			&& n_rec.t < vec3_length(vec3_elem_op(scene->light.pos, '-', n_r.ori)))
		{
			// ambi = vec3_scale(0.f, ambi);
			diff = vec3_scale(0.f, diff);
		}

		return (vec3_elem_op(vec3_elem_op(ambi, '+', diff), '*', rec.c));
	}

	t_vec3  unit_dir = vec3_normalize(r.dir);
	float   a = .5f * (unit_dir.y + 1.f);
	return (vec3_elem_op(vec3_scale(1.f - a, (t_vec3){1.f, 1.f, 1.f}), '+',
		vec3_scale(a, (t_vec3){.5f, .7f, 1.f})));
}

void	initialize(t_scene *scene, t_cam *cam)
{
	cam->i_height = (int) roundf(cam->i_width / cam->aspect);
	cam->i_height = (cam->i_height < 1) + (cam->i_height >= 1) * cam->i_height;

	cam->center = scene->camera.pos;

	float	focal_length = vec3_length(cam->center);
	focal_length = (focal_length < 2.f) * 2.f + (focal_length >= 2.f) * focal_length;
	float	theta = scene->camera.param[0] / 180.f * M_PI;
	float	h = tan(theta / 2.f) / cam->aspect;
	float	v_height = 2.f * h * focal_length;
	float	v_width = v_height * ((float)cam->i_width / cam->i_height);

	cam->vecs[2] = vec3_scale(-1.f, vec3_normalize(scene->camera.axis));
	cam->vecs[0] = vec3_elem_op((t_vec3){0, 1, 0}, 'x', cam->vecs[2]);
	cam->vecs[1] = vec3_elem_op(cam->vecs[2], 'x', cam->vecs[0]);

	t_vec3	v_u = vec3_scale(v_width, cam->vecs[0]);
	t_vec3	v_v = vec3_scale(-v_height, cam->vecs[1]);

	// Calculate the horizontal and vertical delta vectors from pixel to pixel.
	cam->delta_u = vec3_scale(1.f / cam->i_width, v_u);
	cam->delta_v = vec3_scale(1.f / cam->i_height, v_v);

	// Calculate the location of the upper left pixel.
	t_vec3	v_up_left;

	v_up_left = vec3_elem_op(cam->center, '-',
		vec3_elem_op(vec3_scale(focal_length, cam->vecs[2]), '+',
		vec3_scale(0.5f, vec3_elem_op(v_u, '+', v_v))));
	cam->p00_loc = vec3_elem_op(v_up_left, '+',
		vec3_scale(0.5f, vec3_elem_op(cam->delta_u, '+', cam->delta_v)));
}

void	set_px_color(mlx_image_t *img, t_cam *cam, int *pos, t_vec3 color)
{
	uint8_t	*px;
	t_vec3	p_color = vec3_scale(1.f / (float) cam->samples, color);

	px = img->pixels + (pos[0] + pos[1] * img->width) * sizeof(int32_t);
	*px = (int)(inter_clamp((float []){0.f, .999f}, to_gamma(p_color.r)) * 256.f);
	px++;
	*px = (int)(inter_clamp((float []){0.f, .999f}, to_gamma(p_color.g)) * 256.f);;
	px++;
	*px = (int)(inter_clamp((float []){0.f, .999f}, to_gamma(p_color.b)) * 256.f);;
	px++;
	*px = 255;
}

t_vec3	px_sample_square(t_cam *cam)
{
	float	px = -.5f + randf();
	float	py = -.5f + randf();

	return (vec3_elem_op(vec3_scale(px, cam->delta_u), '+',
		vec3_scale(py, cam->delta_v)));
}

t_ray	get_ray(t_cam *cam, int i, int j)
{
	t_vec3	p_center;
	t_vec3	p_sample;
	t_vec3	r_dir;

	p_center = vec3_elem_op(cam->p00_loc, '+',
		vec3_elem_op(vec3_scale(i, cam->delta_u), '+', vec3_scale(j, cam->delta_v)));
	p_sample = vec3_elem_op(p_center, '+', px_sample_square(cam));

	r_dir = vec3_elem_op(p_sample, '+', vec3_scale(-1, cam->center));
	return ((t_ray){cam->center, vec3_normalize(r_dir)});
}

void	render(t_cam *cam, t_scene *scene, mlx_image_t *img)
{
	for (int j = 0; j < cam->i_height; ++j) {
		for (int i = 0; i < cam->i_width; ++i) {
			t_vec3	p_color = (t_vec3){0, 0, 0};
			for (int sample = 0; sample < cam->samples; sample++) {
				t_ray	r = get_ray(cam, i, j);
				p_color = vec3_elem_op(p_color, '+', r_color(r, cam->max_depth, scene));
			}
			set_px_color(img, cam, (int []){i, j}, p_color);
		}
	}
}

int	main()
{
	t_scene	scene;

	scene = (t_scene){0};
	save_objects(&scene, "A 0.2 255,255,255");
	save_objects(&scene, "C 0,0,5 0,0,-1 50");
	save_objects(&scene, "L 2,2,1 0.7 255,0,255");
	save_objects(&scene, "sp 1,0,-1 1.0 255,255,0");
	save_objects(&scene, "pl 0,-1,0 0,1,0 0,0,255");
	save_objects(&scene, "cy 0,0,-1.5 0,1,-1 1.0 1.0 255,0,0");

	t_cam	cam = (t_cam){16.f / 9.f, 400, .samples = 50, .max_depth = 10};
	initialize(&scene, &cam);

	mlx_t		*mlx = mlx_init(cam.i_width, cam.i_height, "miniRT", true);
	mlx_image_t	*img = mlx_new_image(mlx, cam.i_width, cam.i_height);

	memset(img->pixels, 180, img->width * img->height * sizeof(int32_t));

	render(&cam, &scene, img);

	mlx_image_to_window(mlx, img, 0, 0);
	mlx_loop(mlx);
}
