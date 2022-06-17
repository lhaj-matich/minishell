#include "../includes/minishell.h"

void pwd_function (env_list_t *env_list)
{
    env_list_t *tmp;
    char *buffer;

    buffer = NULL;
    buffer = getcwd (NULL, 0);
    if (buffer)
        printf ("%s\n", buffer);
    else
    {
        tmp = env_list;
        while (tmp)
        {
            if (!strcmp (tmp->variable_name, "PWD"))
                printf ("%s\n", tmp->value);
            tmp = tmp->next;
        }
    }
}

int cd_function (char *arg, int flag, env_list_t *env_list)
{
    int i;
    int err;

    i = 0;
    err = 0;
    if (flag == -1)
        write (2 ,"cd: Arguments Err \n", 19);
    else
    {
        if (arg == NULL)
            cd_to_home (env_list);
        else if (!strcmp (arg, "~"))
            cd_to_home (env_list);
        else
        {
            err = chdir (arg);
            if (err == -1)
            {
                err  = check_for_dots (arg, env_list);
                if (err == -1)
                    printf ("no such file or directory : %s\n", arg);
            }
        }
        update_pwd_env (env_list);
    }
    return (0);
}

int echo_function (char *argv[], int argc)
{
    int i = 1;
    char thrilling;

    thrilling = '\0';
    if (strcmp (argv[i], "-n"))
        thrilling = '\n';
    else
        i++;

    while (argv[i])
    {
        if (i == (argc - 1))
            printf ("%s", argv[i]);
        else
            printf ("%s ", argv[i]);
        i++;
    }
    printf ("%c", thrilling);
    return (0);
}