/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minirt_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/19 06:36:22 by rraharja          #+#    #+#             */
/*   Updated: 2023/11/19 06:36:22 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

#include "utils.h"
#include "types_c.h"
#include "types_b.h"
#include "callback.h"
#include "parser_c.h"
#include "parser_b.h"
#include "renderer_b.h"

#include <fcntl.h>

static int	parse_error(int m, int n, char *line)
{
	int		i;
	char	*endl;
	char	buf[512];

	n *= n > 0;
	endl = ft_strchr(line, '\n');
	if (endl)
		*endl = '\0';
	i = -1;
	while (++i < n || i >= 510 || ft_isspace(line[i]))
		buf[i] = ft_isspace(line[i]) * line[i] + !ft_isspace(line[i]) * ' ';
	ft_memcpy(&buf[i], "^", 2);
	return (error_msg(E_PRS, m, line, buf));
}

static int	setup_scene(t_rtx *rtx, int fd)
{
	int		i;
	int		n;
	char	*line;

	i = 0;
	line = get_next_line(fd);
	while (line && ++i)
	{
		rtx->err[0] = '\0';
		n = save_objects(rtx, &rtx->scene, line);
		if (n)
			parse_error(i, n, line);
		if (rtx->err[0])
			error_msg(rtx->err);
		free(line);
		if (n)
		{
			get_next_line(-fd);
			return (1);
		}
		line = get_next_line(fd);
	}
	return (0);
}

static int	setup_config(t_rtx *rtx)
{
	t_vec3	v;

	if (!rtx->size[0] && !rtx->size[1])
	{
		printf(I_SDF, WIDTH, HEIGHT);
		ft_memcpy(rtx->size, (int []){WIDTH, HEIGHT}, 2 * sizeof(int));
	}
	if (setup_shader_program(rtx))
		return (error_msg(E_SHP));
	if (setup_framebuffer(rtx))
		return (error_msg(E_FNC));
	if (setup_buffer_objects(rtx))
		return (error_msg(E_BFO));
	load_texture(rtx);
	load_scene_to_buffer(rtx);
	v = rtx->scene.camera.axis;
	rtx->pitch = M_PI_2 - acos(v.y / sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
	rtx->yaw = (((v.z > 0) - (v.z < 0))
			* acos(v.x / sqrt(v.x * v.x + v.z * v.z)));
	return (0);
}

int	main(int narg, char **args)
{
	int		fd;
	t_rtx	rtx;

	rtx = (t_rtx){.refresh = true};
	if (narg != 2)
		return (error_msg(E_USG));
	fd = open(args[1], O_RDONLY);
	if (fd < 0)
		return (error_msg(E_OPN, args[1]));
	rtx.mlx = mlx_init(WIDTH, HEIGHT, "miniRT", true);
	if (setup_scene(&rtx, fd) || setup_config(&rtx))
		mlx_close_window(rtx.mlx);
	close(fd);
	if (rtx.size[0] && rtx.size[1])
		mlx_set_window_size(rtx.mlx, rtx.size[0], rtx.size[1]);
	mlx_key_hook(rtx.mlx, key_hook, &rtx);
	mlx_mouse_hook(rtx.mlx, mouse_hook, &rtx);
	mlx_cursor_hook(rtx.mlx, cursor_hook, &rtx);
	mlx_scroll_hook(rtx.mlx, scroll_hook, &rtx);
	mlx_resize_hook(rtx.mlx, resize_hook, &rtx);
	mlx_loop_hook(rtx.mlx, loop_hook, &rtx);
	mlx_loop(rtx.mlx);
	mlx_terminate(rtx.mlx);
}
