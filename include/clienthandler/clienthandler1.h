/*

#ifndef _CLIENT_HANDLER1_H
#define _CLIENT_HANDLER1_H

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../mailbox/mailbox.h"

#ifdef __cplusplus
#define __BEGIN_CDECLS  extern "C" {
#define __END_CDECLS    }
#else
#define __BEGIN_CDECLS
#define __END_CDECLS
#endif


__BEGIN_CDECLS

typedef void (*callbackFunctionPtr)(mailbox_data_t*);

typedef struct _uint1024_t
{
	bool seq[1024];
} uint1024_t;

typedef struct _client_handler
{
	pthread_mutex_t* mtx;
	pthread_cond_t* cv;
	mailbox_t* commandMailbox;
	mailbox_t* responseMailbox;
	uint1024_t* sequenceId;
	int* sequenceIdIndex;
	bool* shouldExit;

} client_handler;

void client_handler_init(client_handler* clientHandler, int numberOfClients);
void sendCommand(client_handler* clientHandler, int clientNumber, mailbox_data_t* command, callbackFunctionPtr callbackFunction);
void server(client_handler* clientHandler, int clientNumber);
void exitClient(client_handler* clientHandler, int clientNumber);
void callbackFunction(client_handler* clientHandler, mailbox_data_t* response);

__END_CDECLS


#endif
*/

/*

#include <stdio.h>
#include "include/mailbox/mailbox.h"
#include "include/clienthandler/clienthandler1.h"
#include "include/processcommand/processcommand.h"

void client_handler_init(client_handler* clientHandler, int numberOfClients)
{
	clientHandler->mtx = (pthread_mutex_t*)malloc(numberOfClients * sizeof(pthread_mutex_t));
	clientHandler->cv = (pthread_cond_t*)malloc(numberOfClients * sizeof(pthread_cond_t));
	clientHandler->commandMailbox = (mailbox_t*)malloc(numberOfClients * sizeof(mailbox_t));
	clientHandler->responseMailbox = (mailbox_t*)malloc(numberOfClients * sizeof(mailbox_t));
	clientHandler->sequenceId = (uint1024_t*)malloc(numberOfClients * sizeof(uint1024_t));
	clientHandler->sequenceIdIndex = (int*)malloc(numberOfClients * sizeof(int));
	clientHandler->shouldExit = (bool*)malloc(numberOfClients * sizeof(bool));

	for (int i = 0; i < numberOfClients; i++)
	{
		for (int j = 0; j < 1024; j++)
		{
			clientHandler->sequenceId[i].seq[j] = false;
		}
		mailbox_init(&clientHandler->commandMailbox[i]);
		mailbox_init(&clientHandler->responseMailbox[i]);
		clientHandler->sequenceIdIndex[i] = 0;
		clientHandler->shouldExit[i] = false;
	}
}

void sendCommand(client_handler* clientHandler, int clientNumber, mailbox_data_t* command, callbackFunctionPtr callbackFunction)
{
	pthread_mutex_lock(clientHandler->mtx[clientNumber]);
	printf("Client sent the command: ");
	printData(command);
	int ret;
	ret = mailbox_queue_data(&clientHandler->commandMailbox[clientNumber], command);
	clientHandler->sequenceId[clientNumber].seq[clientHandler->sequenceIdIndex[clientNumber]] = true;
	pthread_cond_signal(clientHandler->cv[clientNumber]);					// checkpoint while debugging: notifyone or notifyall

	while (clientHandler->sequenceId[clientNumber].seq[clientHandler->sequenceIdIndex[clientNumber]])
	{
		pthread_cond_wait(&clientHandler->cv[clientNumber], clientHandler->mtx[clientNumber]);
	}
	mailbox_data_t* response = (mailbox_data_t*)malloc(sizeof(mailbox_data_t));
	while (true)
	{
		ret = mailbox_dequeue_data(clientHandler->responseMailbox, response);
		int seqId;
		seqId = (((response->header) >> 8) & 1023);
		if (seqId == clientHandler->sequenceIdIndex[clientNumber])
		{
			break;
		}
	}
	clientHandler->sequenceIdIndex[clientNumber] = (clientHandler->sequenceIdIndex[clientNumber] + 1) % 1023;
	callbackFunction(clientHandler, response);
}

void server(client_handler* clientHandler, int clientNumber)
{
	while (true)
	{
		mailbox_data_t* command;
		command = (mailbox_data_t*)malloc(sizeof(mailbox_data_t));
		pthread_mutex_lock(clientHandler->mtx[clientNumber]);

		while ((clientHandler->commandMailbox[clientNumber].head == clientHandler->commandMailbox[clientNumber].tail) && (clientHandler->shouldExit[clientNumber] == false))
		{
			pthread_cond_wait(&clientHandler->cv[clientNumber], clientHandler->mtx[clientNumber]);
		}

		if (clientHandler->shouldExit[clientNumber])
		{
			pthread_cond_signal(clientHandler->cv[clientNumber]);
			break;
		}

		int ret;
		ret = mailbox_dequeue_data(clientHandler->commandMailbox, command);
		printf("Server recieved the command: ");
		printData(command);
		pthread_mutex_unlock(clientHandler->mtx[clientNumber]);

		usleep(500 * 1000);

		pthread_mutex_lock(clientHandler->mtx[clientNumber]);
		mailbox_data_t* response;
		response = processCommand(command);
		ret = mailbox_queue_data(clientHandler->responseMailbox, response);
		printf("Server sent the response: ");
		printData(response);
		clientHandler->sequenceId[clientNumber].seq[clientHandler->sequenceIdIndex[clientNumber]] = false;
		pthread_mutex_unlock(clientHandler->mtx[clientNumber]);
		pthread_cond_signal(clientHandler->cv[clientNumber]);
	}
}

void exitClient(client_handler* clientHandler, int clientNumber)
{
	clientHandler->shouldExit[clientNumber] = true;
	pthread_cond_signal(clientHandler->cv[clientNumber]);
}

void callbackFunction(client_handler* clientHandler, mailbox_data_t* response)
{
	printf("Client recieved the response: ");
	printData(response);
}
*/