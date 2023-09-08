#include <iostream>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "include/mailbox/mailbox.h"
#include "include/clienthandler/clienthandler.h"
#include "include/processcommand/processcommand.h"
using namespace std;

void callbackFunction(mailbox_data_t* response)
{
	cout << "Client recieved the response:\t\t";
	printData(response);
}

bool helper(client_handler* clientHandler, int clientNumber)
{
	return ((clientHandler->commandMailbox[clientNumber].head == clientHandler->commandMailbox[clientNumber].tail) && (clientHandler->shouldExit[clientNumber] == false));
}

void _client_handler::sendCommand(int clientNumber, mailbox_data_t* command, function<void(mailbox_data_t*)>callbackFunction)
{
	unique_lock<mutex> lock(mtx[clientNumber]);
	cout << "Client sent the command:\t\t";
	uint32_t seqIdIndex = sequenceIdIndex[clientNumber];
	command->header += (seqIdIndex << 8);
	printData(command);
	int ret;
	ret = mailbox_queue_data(&commandMailbox[clientNumber], command);
	sequenceId[clientNumber].seq[sequenceIdIndex[clientNumber]] = true;
	bool* temp = &(sequenceId[clientNumber].seq[sequenceIdIndex[clientNumber]]);
	cv[clientNumber].notify_one();					// checkpoint while debugging: notifyone or notifyall

	this_thread::sleep_for(chrono::milliseconds(500));

	cv[clientNumber].wait(lock, [temp] { return *temp ? false : true;});
	mailbox_data_t* response = new mailbox_data_t;
	while (true)
	{
		ret = mailbox_dequeue_data(responseMailbox, response);
		int seqId;
		seqId = ((response->header >> 8) & 1023);
		if (seqId == sequenceIdIndex[clientNumber])
		{
			break;
		}
	}
	sequenceIdIndex[clientNumber]++;
	sequenceIdIndex[clientNumber] %= 1023;
	callbackFunction(response);
	lock.unlock();

	this_thread::sleep_for(chrono::milliseconds(2500));
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
		cout << "Server recieved the command:\t\t";
		printData(command);
		lock.unlock();

		this_thread::sleep_for(chrono::milliseconds(500));

		lock.lock();
		mailbox_data_t* response;
		response = processCommand(command);
		ret = mailbox_queue_data(responseMailbox, response);
		cout << "Server sent the response:\t\t";
		printData(response);
		sequenceId[clientNumber].seq[sequenceIdIndex[clientNumber]] = false;
		cv[clientNumber].notify_one();

		this_thread::sleep_for(chrono::milliseconds(500));
	}
}

void _client_handler::exitClient(int clientNumber)
{
	shouldExit[clientNumber] = true;
	cv[clientNumber].notify_one();
}