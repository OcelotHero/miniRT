/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 11:27:03 by rraharja          #+#    #+#             */
/*   Updated: 2022/10/26 11:27:03 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

/*	Wrapper function for free */
char	*ft_free(char **str)
{
	if (str && *str)
	{
		free(*str);
		*str = NULL;
	}
	return (NULL);
}

/*	Wrapper function for read */
int	ft_fd_reader(int fd, char *buffer)
{
	int	n;

	if (fd < 0)
		return (fd);
	n = read(fd, buffer, BUFFER_SIZE);
	if (n != -1)
		buffer[n] = '\0';
	return (n);
}

/*	Extracts n characters from buffer, joining them with the str parameter to
 *	a new allocated string and shifts the remaining characters to the front, i.e.
 *	simultaneous substr & memmove of buffer and strjoin of str with the substr.
 */
char	*ft_extract_join(char *str, char *buffer, int n)
{
	int		i;
	int		len1;
	int		len2;
	char	*res;

	len1 = ft_strlen(str);
	len2 = ft_strlen(buffer);
	res = malloc(sizeof(res) * (len1 + n + 1));
	if (!res)
		return (ft_free(&str));
	res[len1 + n] = '\0';
	i = 0;
	while (i - len1 < n || (n + i - len1 <= len2 && buffer[n + i - len1]))
	{
		if (i < len1)
			res[i] = str[i];
		if (i >= len1 && i - len1 < n)
			res[i] = buffer[i - len1];
		if (i >= len1 && n + i - len1 <= len2)
			buffer[i - len1] = buffer[n + i - len1];
		i++;
	}
	buffer[i - len1] = '\0';
	ft_free(&str);
	return (res);
}

/*	Processes the buffer returned from reading the text contained in fd.
 *	Recursively calling itself until either the buffer contains the new
 *	line character, no more characters to read from fd or an error occured.
 */
char	*ft_processor(int fd, char *str, char **buffer)
{
	int		n;
	char	*tmp;

	n = 0;
	while ((*buffer)[n] && (*buffer)[n] != '\n')
		n++;
	n = n * ((*buffer)[n] == '\n') - !((*buffer)[n]);
	if (n != -1 && fd >= 0)
		return (ft_extract_join(str, *buffer, n + 1));
	tmp = ft_strjoin(str, *buffer);
	ft_free(&str);
	if (!tmp)
		return (ft_free(buffer));
	n = ft_fd_reader(fd, *buffer);
	if (n <= 0)
	{
		ft_free(buffer);
		if (!*tmp || n != 0)
			return (ft_free(&tmp));
		return (tmp);
	}
	return (ft_processor(fd, tmp, buffer));
}

/*	Reads texts from the file descriptor, one line at a time. Negative fd
 *	forces the buffer to be freed if applicable.
 */
char	*get_next_line(int fd)
{
	static char	*buffer;

	if ((fd < 0 || BUFFER_SIZE <= 0) && !buffer)
		return (NULL);
	if (!buffer)
	{
		buffer = malloc(sizeof(*buffer) * (BUFFER_SIZE + 1));
		if (!buffer)
			return (NULL);
		if (ft_fd_reader(fd, buffer) <= 0)
			return (ft_free(&buffer));
	}
	return (ft_processor(fd, ft_strjoin("", ""), &buffer));
}
