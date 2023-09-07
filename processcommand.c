#include "include/processcommand/processcommand.h"
#include <stdlib.h>

mailbox_data_t* processCommand(mailbox_data_t* command)
{
	mailbox_data_t* response;
	response = (mailbox_data_t*)malloc(sizeof(mailbox_data_t));
	*response = *command;
	return response;
}