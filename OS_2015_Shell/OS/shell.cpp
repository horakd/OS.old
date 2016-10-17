#include "shell.h"

#define CMD_CNT 7

std::string commandNames[CMD_CNT];

int(*commands[CMD_CNT])(std::string params, FileH *input, FileH *output, FileH *error);


int shell_validate_command(std::string cmd)
{
	for (int i = 0; i < CMD_CNT; i++)
		if (commandNames[i].compare(cmd) == 0)
			return i;
	return -1;
}

int shell_cmd_echo(std::string params, FileH *input, FileH *output, FileH *error)
{
	for (char ch : params)
	{
		OSCall *call = (OSCall*)malloc(sizeof(OSCall));
		RWContainer *c = (RWContainer*)malloc(sizeof(RWContainer));
		c->character = ch;
		c->handler = output;
		call->params = (void*)c;
		call->service = 2;
		k_syscall(call);
	}

	return 0;
}

void shell_init_commands()
{
	commandNames[0] = "echo";
	commands[0] = &shell_cmd_echo;
	commandNames[1] = "ls";
	commandNames[2] = "pwd";
	commandNames[3] = "cat";
	commandNames[4] = "cd";
	commandNames[5] = "wc";
	commandNames[6] = "mkdir";
}
