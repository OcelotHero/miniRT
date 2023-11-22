/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types_c.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykerdel <ykerdel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/11 09:35:18 by rraharja          #+#    #+#             */
/*   Updated: 2023/11/22 22:13:36 by ykerdel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPES_C_H
# define TYPES_C_H

# ifdef __APPLE__
#  define DENSITY 2.0f
# else
#  define DENSITY 1.0f
# endif

# define MAX_SIZE	256

# define VECS	1
# define UIN8	2
# define NORM	3

# define FLTS	4
# define PCNT	5
# define ANGL	6
# define OAGL	7

# define FOFV	ANGL
# define ICON	ANGL
# define OCON	OAGL
# define INTN	PCNT

# define COLR	UIN8
# define AXIS	NORM
# define NRML	NORM

# define RED	""
# define END	""

# define E_OPN	"Unable to open %s\n"
# define E_PRS	"Error parsing the following line:\n%3d\t%s\n\t\033[31;1m%s\
\033[0m\n"

# ifndef BNS
#  define BNS	0
# endif

enum e_token_types {
	INVAL	= (0x0),
	SIZE	= (0x1 << 27 | FLTS << 9 | FLTS << 6),
	CAMERA	= (0x2 << 27 | VECS << 24 | AXIS << 21 | FOFV << 12),
	QMAP	= (0x3 << 27 | INTN << 12 | COLR),
	AMBI	= (0x4 << 27 | INTN << 12 | COLR),
	PTLGHT	= (0x5 << 27 | VECS << 24 | INTN << 12 | COLR),
	SPLGHT	= (0x6 << 27 | VECS << 24 | AXIS << 21 | ICON << 18 | OCON << 15
		| INTN << 12 | COLR),
	SPHERE	= (0x7 << 27 | VECS << 24 | FLTS << 9 | COLR),
	PLANE	= (0x8 << 27 | VECS << 24 | NRML << 21 | COLR),
	CYLND	= (0x9 << 27 | VECS << 24 | AXIS << 21 | FLTS << 9 | FLTS << 6
		| COLR),
	BOX		= (0xa << 27 | VECS << 24 | AXIS << 21 | FLTS << 12 | VECS << 9
		| COLR),
	CONE	= (0xb << 27 | VECS << 24 | AXIS << 21 | FLTS << 9 | FLTS << 6
		| FLTS << 3 | COLR),
	TRIAGL	= (0xc << 27 | VECS << 24 | VECS << 21 | VECS << 9 | COLR),
	QUAD	= (0xd << 27 | VECS << 24 | VECS << 21 | VECS << 9 | COLR),
	DISK	= (0xe << 27 | VECS << 24 | NRML << 21 | FLTS << 9 | COLR),
};

typedef union s_vec4 {
	struct {
		float	x;
		float	y;
		float	z;
		float	w;
	};
	struct {
		float	r;
		float	g;
		float	b;
		float	a;
	};
	float	e[4];
}	t_vec4;

typedef union s_vec3 {
	struct {
		float	x;
		float	y;
		float	z;
	};
	struct {
		float	r;
		float	g;
		float	b;
	};
	float	e[3];
}	t_vec3;

typedef struct s_ray {
	t_vec3	ori;
	t_vec3	dir;
}	t_ray;

#endif