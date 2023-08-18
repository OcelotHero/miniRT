/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_dprintf.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 19:09:30 by rraharja          #+#    #+#             */
/*   Updated: 2022/10/20 19:09:30 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_DPRINTF_H
# define FT_DPRINTF_H

/*------ Macros ------*/
# define FLAG " -+0#"
# define BASE "0123456789abcdef"
# define CONVERSION "cspdiuxX%"

# include <stdarg.h>
# include <stdbool.h>
# include <stdlib.h>
# include <unistd.h>

/*------ Configurations ------*/
typedef struct s_pconfig {
	bool	form;
	char	padding;
	char	pos_prefix;
	int		width;
	int		precision;
	char	conv;
}	t_pconfig;

/*------ Parser functions ------*/
size_t		ft_parse(int fd, const char **format, va_list args);
t_pconfig	*ft_parse_config(const char **format);

/*------ Printer functions ------*/
size_t		ft_dprint_percent(int fd, t_pconfig *config);
size_t		ft_dprint_null(int fd, t_pconfig *config, bool is_ptr);
size_t		ft_dprint_char(int fd, const char c, t_pconfig *config);
size_t		ft_dprint_nb_base(int fd, long long n, t_pconfig *config);
size_t		ft_dprint_ptr(int fd, unsigned long long ptr, t_pconfig *config);
size_t		ft_dprint_str(int fd, const char *str, char padding,
				t_pconfig *config);
size_t		ft_dprint_none(int fd, t_pconfig *config, const char **format,
				char *start);

/*------ Utils functions ------*/
int			ft_isdigit(int c);
bool		ft_contains(const char *s, char c);
void		ft_bzero(void *s, size_t n);
void		*ft_calloc(size_t count, size_t size);
size_t		ft_strlen(const char *s);
size_t		ft_pos_atoi(const char **str);

/*------ ft_printf function ------*/
int			ft_dprintf(int fd, const char *format, ...);
int			ft_vdprintf(int fd, const char *format, va_list args);

#endif
