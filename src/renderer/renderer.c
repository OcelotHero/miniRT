/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   renderer.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/10 12:46:13 by rraharja          #+#    #+#             */
/*   Updated: 2023/09/11 09:39:16 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define	WIDTH 400

#include "MLX42/MLX42.h"
#include "types.h"
#include "libft.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

t_vec3	vec3_elem_op(t_vec3 a, char op, t_vec3 b);
t_vec3	vec3_scale(float a, t_vec3 v);
t_vec3	vec3_normalize(t_vec3 a);
float	vec3_dot(t_vec3 a, t_vec3 b);
float	vec3_length(t_vec3 a);

t_vec3	ray_at(float t, t_ray ray);

int	save_objects(t_scene *scene, char *str);

typedef struct	s_hit_record {
	bool	front_face;
    float	t;
	t_vec3	p;
    t_vec3	normal;
	t_vec3	color;
}	t_hit_record;

typedef struct	s_cam
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
	while(++i < 3)
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
    return sqrt(linear);
}

float	randf()
{
	return (rand() / (RAND_MAX + 1.0f));
}

float	randfi(float min, float max)
{
	return (min + (max - min) * randf());
}

t_vec3	random_unit_vector()
{
	float	theta = inter_clamp((float []){-M_PI, M_PI}, randfi(-M_PI, M_PI));
	float	phi = inter_clamp((float []){-M_PI_2, M_PI_2}, randfi(-M_PI_2, M_PI_2));

	return ((t_vec3){cos(phi) * cos(theta), cos(phi) * sin(theta), sin(phi)});
}

t_vec3	random_on_hemisphere(t_vec3 normal) {
    t_vec3	on_unit_sphere = random_unit_vector();
	bool	face = vec3_dot(on_unit_sphere, normal) > 0;

	return (vec3_scale((2 * face) - 1, on_unit_sphere));
}

bool	obj_hit(t_ray r, float *r_lim, t_hit_record *rec, t_object *obj)
{
	if (obj->type & (SPHERE & ~0xff))
	{
		t_vec3	oc = vec3_elem_op(r.ori, '-', obj->pos);
		float	half_b = vec3_dot(oc, r.dir);
		float	c = vec3_dot(oc, oc) - obj->param[1] * obj->param[1];

		float	dis = half_b * half_b - c;
		if (dis < 0)
			return (false);

		float	sqrtd = sqrt(dis);
		float	root = (-half_b - sqrtd);
		if (root <= r_lim[0])
			root = (-half_b + sqrtd);
		if (root <= r_lim[0] || r_lim[1] <= root)
			return (false);

		rec->t = root;
		rec->p = ray_at(root, r);
		t_vec3	out_normal = vec3_scale(1.f / obj->param[1],
			vec3_elem_op(oc, '+', vec3_scale(root, r.dir)));
		rec->front_face = vec3_dot(r.dir, out_normal) < 0;
		rec->normal = vec3_scale((2 * rec->front_face) - 1, out_normal);
		rec->color = vec3_scale(1.f / 255.f, obj->color);
		return (true);
	}
	else if (obj->type & (PLANE & ~0xff))
	{
		t_vec3	oc = vec3_elem_op(r.ori, '-', obj->pos);
		float	a = vec3_dot(r.dir, obj->axis);
		float	b = vec3_dot(oc, obj->axis);

		float	root = -b / a;
		if (root <= r_lim[0] || root > r_lim[1])
			return (false);

		rec->t = root;
		rec->p = ray_at(root, r);
		rec->front_face = vec3_dot(r.dir, obj->axis) < 0;
		rec->normal = vec3_scale((2 * rec->front_face) - 1, obj->axis);
		rec->color = vec3_scale(1.f / 255.f, obj->color);
		return (true);
	}
	else if (obj->type & (CYLND & ~0xff))
	{
		t_vec3	nv = vec3_normalize(obj->axis);
		t_vec3	oc = vec3_elem_op(r.ori, '-', obj->pos);
		float	nvrd = vec3_dot(nv, r.dir);
		float	nvoc = vec3_dot(nv, oc);
		float	a = 1.f - nvrd * nvrd;
		float	half_b = vec3_dot(oc, r.dir) - nvoc * nvrd;
		float	c = vec3_dot(oc, oc) - nvoc * nvoc - obj->param[1] * obj->param[1];
		float	dis = half_b * half_b - a * c;
		if (dis < 0)
			return (false);

		float	sqrtd = sqrt(dis);
		float	root = (-half_b - sqrtd) / a;
		if (root <= r_lim[0])
			root = (-half_b + sqrtd) / a;
		if (root <= r_lim[0] || root >= r_lim[1])
			return (false);

		float	h = nvoc + root * nvrd;
		if (fabs(h) <= obj->param[2])
		{
			rec->t = root;
			rec->p = ray_at(root, r);
			t_vec3	out_normal = vec3_scale(1.f / obj->param[1], vec3_elem_op(vec3_elem_op(oc, '+', vec3_scale(root, r.dir)), '+', vec3_scale(-h, nv)));
			rec->front_face = vec3_dot(r.dir, out_normal) < 0;
			rec->normal = vec3_scale((2 * rec->front_face) - 1, out_normal);
			rec->color = vec3_scale(1.f / 255.f, obj->color);
			return (true);
		}

		root = (((h > 0) - (h <= 0)) * obj->param[2] - nvoc) / nvrd;
		if (fabs(half_b + a * root) >= sqrtd || root <= r_lim[0] || root >= r_lim[1])
			return (false);

		rec->t = root;
		rec->p = ray_at(root, r);
		t_vec3	out_normal = vec3_scale((float)((h > 0) - (h < 0)), nv);
		rec->front_face = vec3_dot(r.dir, out_normal) < 0;
		rec->normal = vec3_scale((2 * rec->front_face) - 1, out_normal);
		rec->color = vec3_scale(1.f / 255.f, obj->color);
		return (true);

		// t_vec3	oc1 = vec3_elem_op(r.ori, '-', vec3_elem_op(obj->pos, '+', vec3_scale(obj->param[2], nv)));
		// float	t1 = -vec3_dot(nv, oc1) / vec3_dot(r.dir, nv);
		// t_vec3	oc2 = vec3_elem_op(r.ori, '-', vec3_elem_op(obj->pos, '+', vec3_scale(-obj->param[2], nv)));
		// float	t2 = -vec3_dot(nv, oc2) / vec3_dot(r.dir, nv);
		// if ((t1 <= r_lim[0] && t2 <= r_lim[0]) || (t1 >= r_lim[1] && t2 >= r_lim[1]))
		// 	return (false);

		// float	t = t1;
		// oc = oc1;
		// h = 1;
		// if ((t2 < t && t2 > r_lim[0]) || t <= r_lim[0])
		// {
		// 	oc = oc2;
		// 	t = t2;
		// 	h = -1;
		// }
		// t_vec3	p = vec3_elem_op(oc, '+', vec3_scale(t, r.dir));
		// if (vec3_dot(p, p) > (obj->param[1] * obj->param[1]))
		// 	return (false);

		// rec->t = t;
		// rec->p = ray_at(t, r);
		// t_vec3	out_normal = vec3_scale(h, nv);
		// rec->front_face = vec3_dot(r.dir, out_normal) < 0;
		// rec->normal = vec3_scale((2 * rec->front_face) - 1, out_normal);
		// return (true);
	}
	return (false);
}

bool	hit(t_ray r, float *r_lim, t_hit_record *rec, t_scene *scene)
{
	t_hit_record	temp_rec;
	bool			hit_anything = false;
	float			closest_so_far = r_lim[1];

	for (int i = 0; i < scene->n_obj; i++) {
		if (obj_hit(r, (float []){r_lim[0], closest_so_far}, &temp_rec, &scene->objects[i])) {
			hit_anything = true;
			closest_so_far = temp_rec.t;
			*rec = temp_rec;
		}
	}

	return hit_anything;
}

t_vec3  r_color(t_ray r, int depth, t_scene *scene)
{
	t_hit_record	rec;

	if (depth <= 0)
		return ((t_vec3){0.f, 0.f, 0.f});

	if (hit(r, (float []){.001f, 1.f / 0.f}, &rec, scene)) {
		// t_vec3	dir = vec3_elem_op(rec.normal, '+', random_unit_vector());
		// return (vec3_elem_op((t_vec3){.5f, .5f, 0.f}, '*', r_color((t_ray){rec.p, dir}, --depth, scene)));
		t_vec3	l_dir = vec3_normalize(vec3_elem_op(scene->light.pos, '-', rec.p));
		float	dif = scene->light.param[0] * inter_clamp((float []){0.f, 1.f}, vec3_dot(rec.normal, l_dir));
		t_vec3	diff = vec3_scale(dif, vec3_normalize(scene->light.color));

		t_vec3	ambi = vec3_scale(scene->ambient.param[0], vec3_normalize(scene->ambient.color));

		t_ray	n_r = (t_ray){rec.p, l_dir};
		t_hit_record	n_rec;
		if (hit(n_r, (float []){.001f, 1.f / 0.f}, &n_rec, scene)
			&& n_rec.t < vec3_length(vec3_elem_op(scene->light.pos, '-', n_r.ori)))
		{
			// ambi = vec3_scale(0.f, ambi);
			diff = vec3_scale(0.f, diff);
		}

		return (vec3_elem_op(vec3_elem_op(ambi, '+', diff), '*', rec.color));
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
	float	theta = scene->camera.param[3] / 180.f * M_PI;
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

	ft_bzero(&scene, sizeof(scene));
	save_objects(&scene, "A 0.2 255,255,255");
	save_objects(&scene, "C 0,0,5 0,0,-1 50");
	save_objects(&scene, "L 2,2,1 0.7 255,0,255");
	save_objects(&scene, "sp 1,0,-1 0.5 255,255,0");
	save_objects(&scene, "pl 0,-1,0 0,1,0 0,0,255");
	save_objects(&scene, "cy 0,0,-1.5 0,1,-1 0.5 0.5 255,0,0");

	t_cam	cam = (t_cam){16.f / 9.f, 400, .samples = 50, .max_depth = 10};
	initialize(&scene, &cam);

	mlx_t		*mlx = mlx_init(cam.i_width, cam.i_height, "miniRT", true);
	mlx_image_t	*img = mlx_new_image(mlx, cam.i_width, cam.i_height);

	memset(img->pixels, 180, img->width * img->height * sizeof(int32_t));

	render(&cam, &scene, img);

	mlx_image_to_window(mlx, img, 0, 0);
	mlx_loop(mlx);
}
