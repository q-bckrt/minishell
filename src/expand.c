/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qbackaer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/07 01:45:36 by qbackaer          #+#    #+#             */
/*   Updated: 2019/10/10 19:19:08 by qbackaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*glue_arg(char **split)
{
	char	*arg;
	char	*tmp;
	char	**roam;

	arg = strdup_safe("");
	roam = split;
	while (*roam)
	{
		if (ft_strcmp(*roam, " "))
		{
			tmp = arg;
			if (!(arg = ft_strjoin(arg, *roam)))
				exit(EXIT_FAILURE);
			free(tmp);
		}
		roam++;
	}
	return (arg);
}

static char	**get_val(char **split, char **env)
{
	char	**roam;
	char	*tmp;

	roam = split;
	while (*roam)
	{
		if (*roam[0] == '$' && ft_strcmp(*roam, "$"))
		{
			if (!(tmp = get_env_var(env, (*roam) + 1)))
			{
				free(*roam);
				*roam = strdup_safe("");
			}
			else
			{
				free(*roam);
				*roam = strdup_safe(tmp);
				free(tmp);
			}
		}
		roam++;
	}
	return (NULL);
}

static char	**lex_var(char *str)
{
	char			**split;
	unsigned int	i;
	size_t			e;

	i = 0;
	e = 0;
	split = init_tab();
	while (str[i] && str[e])
	{
		while (ft_isalpha(str[e]) && str[e] && e && str[i - 1] == '$')
			e++;
		if (i && str[i - 1] == '$')
			i--;
		realloc_sub(str, &split, i, e);
		i = e;
		while (str[e] && str[e] != '$')
			e++;
		if (i != e)
			realloc_sub(str, &split, i, e);
		if (str[e])
			i = ++e;
	}
	return (split);
}

char		*expand_vars(char *str, char **env)
{
	char	**split;
	char	*expand;

	split = lex_var(str);
	get_val(split, env);
	expand = glue_arg(split);
	ft_free_tab2(split);
	return (expand);
}

char		*expand_tilde(char *str, char *home)
{
	char			*xstr;
	unsigned int	i;
	unsigned int	j;

	if (!(xstr = malloc(ft_strlen(str) + ft_strlen(home))))
		exit(EXIT_FAILURE);
	i = 0;
	j = 0;
	while (str[i])
	{
		while (i == 0 && str[0] == '~' && home[j])
		{
			xstr[j] = home[j];
			j++;
		}
		xstr[i + j] = str[i + 1];
		i++;
	}
	return (xstr);
}
