/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 11:27:16 by rraharja          #+#    #+#             */
/*   Updated: 2022/10/26 11:27:16 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE BUFSIZ
# endif

/*------ Main functions ------*/
int		ft_fd_reader(int fd, char *buffer);
char	*ft_free(char **str);
char	*ft_processor(int fd, char *str, char **buffer);
char	*ft_extract_join(char *str, char *buffer, int n);
char	*get_next_line(int fd);

/*------ Utils functions ------*/
char	*ft_strjoin(char const *s1, char const *s2);
size_t	ft_strlen(const char *str);
#endif
