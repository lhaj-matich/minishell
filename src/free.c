/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ochoumou <ochoumou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/02 14:13:40 by ochoumou          #+#    #+#             */
/*   Updated: 2022/08/04 19:05:24 by ochoumou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	clear_redirections(t_redirect **lst)
{
	t_redirect *tmp_node;

	if (!*lst)
		return ;
	while (*lst)
	{
		tmp_node = *lst;
		*lst = tmp_node->next;
		free(tmp_node->filename);
		free(tmp_node->heredoc_content);
		free(tmp_node);
	}
	*lst = NULL;
}

void	clear_commands(t_cmd **lst)
{
	t_cmd   *tmp_node;

	if (!*lst)
		return ;
	while (*lst)
	{
		tmp_node = *lst;
		*lst = tmp_node->next;
		free(tmp_node->cmd);
		free(tmp_node);
	}
	*lst = NULL;
}

void	free_parser(t_exec_node **lst)
{
	t_exec_node	*tmp_node;

	if (!*lst)
		return ;
	while (*lst)
	{
		tmp_node = *lst;
		*lst = tmp_node->next;
        clear_redirections(&tmp_node->cmd->redir_list);
        clear_commands(&tmp_node->cmd->cmds);
		free(tmp_node->cmd);
		free(tmp_node);
	}
}

void	free_lexer(lexer_node_t **lst)
{
	lexer_node_t *tmp_node;
	if (!*lst)
		return ;
	while (*lst)
	{
		tmp_node = *lst;
		*lst = tmp_node->next;
		free(tmp_node->start);
		free(tmp_node);
	}
	free(*lst);
}

// String related frees

void	free_string_table(char **table)
{
	int i;

	i = 0;
	while (table[i])
		free(table[i]);
	free(table);
}

char    *free_joined(char *str)
{
    char *tmp;

    tmp = str;
    free(str);
    return (tmp);
}