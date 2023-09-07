#include <thread>

#include "include/mailbox/mailbox.h"
#include "include/clienthandler/clienthandler.h"
using namespace std;


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
	uint32_t one = 1;
	command2->header = 5 + (one << 8);
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