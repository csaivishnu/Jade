#include <thread>

#include "include/mailbox/mailbox.h"
#include "include/clienthandler/clienthandler.h"
using namespace std;


void client0(client_handler* clientHandler)
{
	int clientNumber;
	clientNumber = 0;
	thread startServer([clientHandler, clientNumber]() {
		clientHandler->server(clientNumber);
		});


//	first command: size of command: 5

	mailbox_data_t* command1 = new mailbox_data_t;
	command1->header = 5;
	command1->payload[0] = 2;
	clientHandler->sendCommand(0, command1, callbackFunction);


	//	second command: size of command: 5, seq id: 1

	mailbox_data_t* command2 = new mailbox_data_t;
	command2->header = 6;
	command2->payload[0] = 2;
	command2->payload[1] = 4;
	clientHandler->sendCommand(0, command2, callbackFunction);


	//	third command: size of command: 5, seq id: 1

	mailbox_data_t* command3 = new mailbox_data_t;
	command3->header = 7;
	command3->payload[0] = 2;
	command3->payload[1] = 4;
	command3->payload[2] = 6;
	clientHandler->sendCommand(0, command3, callbackFunction);

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