/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihyeole <jihyeole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 11:33:01 by jihyeole          #+#    #+#             */
/*   Updated: 2023/05/21 13:17:49 by jihyeole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	exit_status;

char	*read_command(void)
{
	char	*command;

	command = readline("minishell$ ");
	if (command == NULL)
	{
		ft_putstr_fd("\x1b[1A", 1);
		ft_putstr_fd("\033[11C", 1);
		write(1, "exit\n", 5);
		exit(0);
	}
	if (ft_strlen(command) != 0)
		add_history(command);
	return (command);
}

t_info	*parse_command(char *command, t_env *env_lst)
{
	// 파싱 구현
	// syntax error NULL 반환 
	t_info *info;
	// t_redirect	*re;
	(void)command;
	(void)env_lst;
	// char	**commands;
	// char	**commands1;
	// char	**commands2;

	// re = (t_redirect *)malloc(sizeof(t_redirect) * 2);
	// re[0].file = ft_strdup("EOF");
	// re[0].type = 1;
	// re[0].fd = NULL;
	// re[0].next = NULL;
	// re[1].file = ft_strdup("output");
	// re[1].type = 1;
	// re[1].fd = NULL;
	// re[1].next = NULL;
	// // re[2].file = ft_strdup("abc");
	// // re[2].type = 2;
	// re[2].next = NULL;
	info = (t_info *)malloc(sizeof(t_info));
	info->process_num = 1;
	info->heredoc_num = 1;
	// commands = ft_split(command, ' ');
	info->commands = (t_command *)malloc(sizeof(t_command) * 1);
	(&(info->commands[0]))->command = ft_split(command, ' ');
	(&(info->commands[0]))->output = NULL;
	(&(info->commands[0]))->input = NULL;
	// info->commands = (t_command *)malloc(sizeof(t_command) * 3);
	// info->commands[0].command = ft_split("cat main.c", ' ');
	// info->commands[0].output = NULL;
	// info->commands[0].input = NULL;
	// info->commands[1].command = ft_split("wc", ' ');
	// info->commands[1].output = NULL;
	// info->commands[1].input = NULL;
	// info->commands[2].command = ft_split("cat", ' ');
	// info->commands[2].output = NULL;
	// info->commands[2].input = NULL;
	// printf("%s\n", (info->commands)[0].command[0]);
	return (info);
}

void	print_info(t_info *info)
{
	ft_printf("process_num = %d\n", info->process_num);
	ft_printf("heredoc_num = %d\n", info->heredoc_num);

	int i = 0;
	int j;
	t_command *command;
	t_redirect *input;
	t_redirect *output;
	while (i < info->process_num)
	{
		ft_printf("commands %d\n", i);
		j = 0;
		command = &(info->commands[i]);
		input = command->input;
		output = command->output;
		while (command->command[j])
		{
			ft_printf("%s\n", command->command[j]);
			j++;
		}
		while (input)
		{
			ft_printf("input\n");
			ft_printf("type: %d\n", input->type);
			ft_printf("type: %s\n", input->fd);
			ft_printf("type: %s\n", input->file);
			input = input->next;
		}
		while (output)
		{
			ft_printf("input\n");
			ft_printf("type: %d\n", output->type);
			ft_printf("type: %s\n", output->fd);
			ft_printf("type: %s\n", output->file);
			input = input->next;
		}
		++i;
	}
}

int	main(int argc, char *argv[], char **env)
{
	t_env		*env_lst;
	char		*command;
	char		*add;
	t_info		*info;
	t_process	*process;
	char		*temp;
	int			ret;

	init(argc, argv, env, &env_lst);
	while (1)
	{
		command = read_command();
		if (command == NULL)
			continue ;
		if (command[ft_strlen(command) - 1] == '|' && command[ft_strlen(command) - 2] != '|')
		{
			add = ft_strdup(" |");
			while (add[ft_strlen(add) - 1] == '|' && add[ft_strlen(add) - 2] != '|')
			{
				free(add);
				add = readline("> ");
				temp = command;
				command = ft_strjoin(command, add);
				free(temp);
			}
			free(add);
		}
		if (ft_strncmp(command, "", 1) == 0) //나중에 parse에서 처리 - 아무것도 없으면 NULL로
			continue ;
		info = parse_command(command, env_lst);
		if (info == NULL)
			continue ;
		print_info(info);
		if (create_heredoc_temp(info, env_lst) == 0)
			continue ;
		if (info->process_num == 1)
		{
			ret = exec_single_builtin(info, &env_lst);
			if (ret == 1)
			{
				unlink_heredocs(info);
				exit_status = 0;
				continue ;
			}
			else if (ret == -1)
			{
				unlink_heredocs(info);
				continue ;
			}
		}
		process = (t_process *)malloc(sizeof(t_process) * info->process_num);
		create_pipe(process, info->process_num);
		fork_and_execute(process, info, &env_lst);
		wait_all_child(info->process_num, process);
		unlink_heredocs(info);
		free(command);
	}
}
