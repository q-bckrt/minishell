/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qbackaer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/26 20:30:53 by qbackaer          #+#    #+#             */
/*   Updated: 2019/10/10 19:55:14 by qbackaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int			is_builtin(char **cmd, char ***env, t_pwd *pwd)
{
	if (!ft_strcmp(cmd[0], "exit"))
		return (-1);
	else if (!ft_strcmp(cmd[0], "echo"))
		echo(cmd);
	else if (!ft_strcmp(cmd[0], "cd"))
		cd(cmd, env, pwd);
	else if (!ft_strcmp(cmd[0], "env"))
		printenv(*env);
	else if (!ft_strcmp(cmd[0], "setenv"))
		*env = set_env(cmd, env);
	else if (!ft_strcmp(cmd[0], "unsetenv"))
		*env = unset_env(cmd, env);
	else
		return (0);
	return (1);
}

static int	find_binary(char *dirpath, char *binname)
{
	struct dirent	*de;
	DIR				*dr;

	if (!(dr = opendir(dirpath)))
	{
		ft_putendl_fd("opendir failure", 2);
		return (0);
	}
	while ((de = readdir(dr)))
		if (!ft_strcmp(binname, de->d_name))
		{
			closedir(dr);
			return (1);
		}
	closedir(dr);
	return (0);
}

static int	execute_name(char **path_tab, char **cmd, char **env)
{
	char	**pt_ptr;
	char	*path;
	int		found;

	found = 0;
	pt_ptr = path_tab;
	while (*pt_ptr)
	{
		if (find_binary(*pt_ptr, cmd[0]))
		{
			path = cat_path(*pt_ptr, cmd[0]);
			execute(path, cmd, env);
			free(path);
			found = 1;
			break ;
		}
		pt_ptr++;
	}
	return (found);
}

int			is_binary(char **cmd, char ***env)
{
	char	**path_tab;
	char	*var;
	int		found;

	if (!env || !cmd)
		return (0);
	if (ft_strchr(cmd[0], '/'))
	{
		if (!(execute(cmd[0], cmd, *env)))
			return (0);
		return (1);
	}
	if (!(var = get_env_var(*env, "PATH")))
	{
		ft_putendl_fd("minishell: PATH not set", 2);
		return (-1);
	}
	if (!(path_tab = split_paths(var)))
		exit(EXIT_FAILURE);
	free(var);
	found = execute_name(path_tab, cmd, *env);
	ft_free_tab2(path_tab);
	return (found);
}

int			execute(char *path, char **cmd, char **env)
{
	pid_t		pid;
	struct stat	st_buff;

	if (!path)
		return (0);
	if (lstat(path, &st_buff))
		return (access_error_wpr(path, 0, st_buff));
	if (S_ISREG(st_buff.st_mode) && st_buff.st_mode & S_IXUSR)
	{
		pid = fork();
		signal(SIGINT, psig_handler);
		if (pid == 0)
		{
			if (execve(path, cmd, env))
				exit(EXIT_FAILURE);
			exit(EXIT_SUCCESS);
		}
		if (pid > 0)
			wait(&pid);
		if (pid < 0)
			exit(EXIT_FAILURE);
	}
	else
		return (access_error_wpr(path, 1, st_buff));
	return (1);
}
