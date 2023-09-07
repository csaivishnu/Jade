#ifndef _CLIENT_HANDLER_H
#define _CLIENT_HANDLER_H

#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "../mailbox/mailbox.h"
using namespace std;

typedef struct _uint1024_t
{
	bool seq[1024];
} uint1024_t;

typedef struct _client_handler
{
	mutex* mtx;
	condition_variable* cv;
	mailbox_t* commandMailbox;
	mailbox_t* responseMailbox;
	uint1024_t* sequenceId;
	int* sequenceIdIndex;
	bool* shouldExit;

	_client_handler(int numberOfClients)
	{
		mtx = new mutex[numberOfClients];
		cv = new condition_variable[numberOfClients];
		commandMailbox = new mailbox_t[numberOfClients];
		responseMailbox = new mailbox_t[numberOfClients];
		sequenceId = new uint1024_t[numberOfClients];
		sequenceIdIndex = new int[numberOfClients];
		shouldExit = new bool[numberOfClients];
		for (int i = 0; i < numberOfClients; i++)
		{
			for (int j = 0; j < 1024; j++)
			{
				sequenceId[i].seq[j] = false;
			}
			mailbox_init(&commandMailbox[i]);
			mailbox_init(&responseMailbox[i]);
			sequenceIdIndex[i] = 0;
			shouldExit[i] = false;
		}
	}
	void sendCommand(int clientNumber, mailbox_data_t* command, function<void(mailbox_data_t*)>callbackFunction);
	void server(int clientNumber);
	void exitClient(int clientNumber);
} client_handler;

void callbackFunction(mailbox_data_t* response);


#endif