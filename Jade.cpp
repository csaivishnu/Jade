#include <iostream>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "include/mailbox/mailbox.h"
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

	_client_handler(int numberOfClients);
	void sendCommand(int clientNumber, mailbox_data_t* command, function<void(mailbox_data_t*)>callbackFunction);
	void server(int clientNumber);
	void exitClient(int clientNumber);
} client_handler;

_client_handler::_client_handler(int numberOfClients)
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

void callbackFunction(mailbox_data_t* response)
{
	cout << "Client recieved the response: ";
	printData(response);
}

bool helper(client_handler* clientHandler, int clientNumber)
{
	return ((clientHandler-> commandMailbox[clientNumber].head == clientHandler->commandMailbox[clientNumber].tail) && (clientHandler->shouldExit[clientNumber] == false));
}

void _client_handler::sendCommand(int clientNumber, mailbox_data_t *command, function<void(mailbox_data_t*)>callbackFunction)
{
	unique_lock<mutex> lock(mtx[clientNumber]);
	cout << "Client sent the command: ";
	printData(command);
	int ret;
	ret = mailbox_queue_data(&commandMailbox[clientNumber], command);
	sequenceId[clientNumber].seq[sequenceIdIndex[clientNumber]] = true;
	bool* temp = &(sequenceId[clientNumber].seq[sequenceIdIndex[clientNumber]]);
	cv[clientNumber].notify_one();					// checkpoint while debugging: notifyone or notifyall

	cv[clientNumber].wait(lock, [temp] { return *temp ? false : true;});
	sequenceIdIndex[clientNumber]++;
	mailbox_data_t* response = new mailbox_data_t;
	ret = mailbox_dequeue_data(responseMailbox, response);
	callbackFunction(response);
}

void _client_handler::server(int clientNumber)
{
	while (true)
	{
		mailbox_data_t* command;
		command = new mailbox_data_t;
		unique_lock<mutex> lock(mtx[clientNumber]);

		cv[clientNumber].wait(lock, [this, clientNumber] { return helper(this, clientNumber) ? false : true; });

		if (shouldExit[clientNumber])
		{
			cv[clientNumber].notify_one();
			break;
		}

		int ret;
		ret = mailbox_dequeue_data(commandMailbox, command);
		cout << "Server recieved the command: ";
		printData(command);
		lock.unlock();

		this_thread::sleep_for(chrono::milliseconds(500));

		lock.lock();
		mailbox_data_t* response;
		response = new mailbox_data_t;
		*response = *command;
		ret = mailbox_queue_data(responseMailbox, response);
		cout << "Server sent the response: ";
		printData(response);
		sequenceId[clientNumber].seq[sequenceIdIndex[clientNumber]] = false;
		lock.unlock();
		cv[clientNumber].notify_one();
	}
}

void _client_handler::exitClient(int clientNumber)
{
	shouldExit[clientNumber] = true;
	cv[clientNumber].notify_one();
}

void client0(client_handler *clientHandler)
{
	int clientNumber;
	clientNumber = 0;
	thread startServer([clientHandler, clientNumber]() {
		clientHandler->server(clientNumber);
	});
	mailbox_data_t *command1 = new mailbox_data_t;
	command1->header = 5;
	command1->payload[0] = 2;
	clientHandler->sendCommand(0, command1, callbackFunction);

	mailbox_data_t* command2 = new mailbox_data_t;
	command2->header = 5;
	command2->payload[0] = 2;
	clientHandler->sendCommand(0, command2, callbackFunction);

	clientHandler->exitClient(0);
	startServer.join();
}

int main()
{
	int numberOfClients = 1;

	client_handler clientHandler(numberOfClients);

	thread clientThread0(client0, &clientHandler);

	clientThread0.join();

	return 0;
}