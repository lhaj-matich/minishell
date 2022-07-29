#include "../includes/minishell.h"

int test_piped_commands (shell_args_t *args)
{
    t_exec_node *tmp;
    int **fds;
    int indx;
    int id;
    int status;

    indx = 0;
    tmp = args->exec_node;
    if (!tmp)
        return (258);
    fds = open_fd_table (nodes_number(args), args);

        while (tmp)
        {
            id = fork_child (args);
            if (!id)
            {
                handle_redirections (args, tmp->cmd->redir_list);
                if (tmp->builtin)
                {
                    status = handle_builtin (args, tmp, fds, indx);
                    exit (status);
                }
                else
                {
                    link_pipes (tmp, fds, indx);
                    exec_command (args, tmp);
                }
            }
            indx++;
            tmp = tmp->next;
        }
    close_fd_table (fds);
    return (0);
}

void link_pipes (t_exec_node *tmp, int **fds, int indx)
{
    if (tmp->prev != NULL && tmp->next != NULL)
        handle_doubly_piped (indx, fds);
    else if (tmp->next == NULL && tmp->prev != NULL)
        handle_last_command (indx, fds);
    else if (tmp->next != NULL && tmp->prev == NULL)
        handle_first_command (indx, fds);
}


int builtin_routine (shell_args_t *args, t_exec_node *exec_node)
{
    char **cmds;
    int exit_status;

    cmds = get_commands (exec_node->cmd->cmds);
    exit_status = 0;
    if (!ft_strcmp (cmds[0], "cd"))
        exit_status  = handle_cd (args, cmds);
    else if (!advanced_strcmp (cmds[0], "pwd"))
        pwd_function (args->env_list);
    else if (!advanced_strcmp (cmds[0], "echo"))
        echo_function (cmds, number_of_el (cmds));
    else if (!advanced_strcmp (cmds[0], "env"))
        ft_env (exec_node, args->env_list, args);
    else if (!advanced_strcmp (cmds[0], "unset"))
        ft_unset (exec_node, &args->env_list, args);
    else if (!advanced_strcmp (cmds[0], "exit"))
        ft_exit (exec_node, args->env_list, args);
    else if (!advanced_strcmp (cmds[0], "export"))
        ft_export(exec_node, args->env_list, args);
    free_tab (cmds);
    return (exit_status);
}

void exec_command (shell_args_t *args, t_exec_node *exec_node)
{
    char **cmds;
    char **path;
    int status;


    status = 0;
    cmds = get_commands (exec_node->cmd->cmds);
    path = get_path (args, cmds, &status);    
    if (!path[0])
        shell_err (cmds[0], status);
    execve (path[0], cmds, args->env);
}

char **check_for_path (char *cmd)
{
    char **path_table;
    char **cmd_path;
    char *dir_name;
    int i;

    path_table = NULL;
    cmd_path = NULL;
    i = 0;
    if (ft_strchr (cmd, '/'))
    {
        path_table = ft_split (cmd, '/');
        if (cmd[0] == '/' )
            cmd_path = handle_absolue_path (path_table);
        else
            cmd_path = handle_relative_path (path_table);
        free_tab (path_table);
    }
    return (cmd_path); 
}


char **handle_absolue_path (char **path_table)
{
    char **cmd_path;
    char *dir_name;
    int i;

    i = 0;
    dir_name = NULL;
    cmd_path = malloc (sizeof (char*) * 3);
    if (!cmd_path)
        allocation_err ();
    cmd_path[2] = NULL;
    cmd_path[0] = ft_strdup ("");
    while (path_table[i])
    {
        if (!path_table[i + 1])
            break;
        dir_name = ft_strjoin ("/", path_table[i]);
        cmd_path[0] = ft_strjoin (cmd_path[0], dir_name);
        i++;
    }
    cmd_path[1] = ft_strdup (path_table[i]);
    return (cmd_path);
}

char **handle_relative_path (char **path_table)
{
    char **cmd_path;
    char *dir_name;
    int i;

    i = 0;
    dir_name = NULL;
    cmd_path = malloc (sizeof (char*) * 3);
    if (!cmd_path)
        allocation_err ();
    cmd_path[2] = NULL;
    cmd_path[0] = ft_strdup ("");
    while (path_table[i])
    {
        if (!path_table[i + 1])
            break;
        if (!i)
            dir_name = path_table[i];
        else
            dir_name = ft_strjoin ("/", path_table[i]);
        cmd_path[0] = ft_strjoin (cmd_path[0], dir_name);
        i++;
    }
    cmd_path[1] = ft_strdup (path_table[i]);
    return (cmd_path);
}

int handle_simple_command (shell_args_t *args)
{
    int id;
    int status;

    status = 0;
    handle_redirections (args, args->exec_node->cmd->redir_list);
    if (args->exec_node->builtin)
        status = builtin_routine (args, args->exec_node);
    else 
    {
        id = fork_child (args);
        if (id == 0)
            exec_command (args, args->exec_node);
    }
    return (status);
}


void shell_err (char *command, int status)
{
    if (status == -126)
    {
        status = 126;
        ft_putstr_fd (ft_strjoin (command, ": is a directory\n"), STDERR_FILENO);
    }
    else if (status == 127)
        ft_putstr_fd (ft_strjoin (command, ": command not found\n"), STDERR_FILENO);
    else    
        ft_putstr_fd (ft_strjoin (command, ":  Permission denied\n"), STDERR_FILENO);
    exit (status);
}

char **get_path (shell_args_t *args, char **cmds, int *status)
{
    char *path;
    char **path_check;
    char **path_table;
    char *cmd;

    path_table = malloc (sizeof (char *) * 3);
    path_check = check_for_path (cmds[0]);
    path = NULL;
    path_table[2] = NULL;
    cmd = cmds[0];
    if (path_check)
    {
        path = path_check[0];
        path_table[1] = path_check[1];
        cmd = path_table[1];
    }
    else
        path_table[1] = cmds[0];
    path = check_access (cmds[0], path, status);
    path_table[0] = path;
    return (path_table);
}


int handle_cd (shell_args_t *args, char **cmds)
{
    int exit_status ;

    exit_status = 0;
    if (number_of_el (cmds) > 2)
        exit_status = cd_function (NULL, -1, &args->env_list);
    else if (number_of_el (cmds) == 2)
        exit_status = cd_function (cmds[1], 1, &args->env_list);
    else 
        exit_status = cd_function (NULL, 1, &args->env_list);
    return (exit_status);
}