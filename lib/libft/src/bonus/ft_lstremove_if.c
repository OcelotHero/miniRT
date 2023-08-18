/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstremove_if.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraharja <rraharja@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/26 07:36:30 by rraharja          #+#    #+#             */
/*   Updated: 2023/07/13 05:06:09 by rraharja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstremove_if(t_list **begin_list, void *data_ref,
							int (*cmp)(), void (*free_fct)(void *))
{
	t_list	*node;

	if (!begin_list || !(*begin_list))
		return ;
	node = *begin_list;
	if (!(*cmp)(node->content, data_ref))
	{
		*begin_list = node->next;
		if (free_fct)
			(*free_fct)(node->content);
		free(node);
		ft_lstremove_if(begin_list, data_ref, cmp, free_fct);
	}
	else
		ft_lstremove_if(&((*begin_list)->next), data_ref, cmp, free_fct);
}
