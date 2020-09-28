#include "PClient.h"
#include <iostream>

void Example_For_Receiving()
{
	// Init.
	PClient client("127.0.0.1", "1444");

	// Connect to target server & assign callback function.
	client.connect("", "", [&client](std::string response)
		{ 
			// Print received message in console.
			std::cout << "[Recv] " << response << std::endl;

			// Send the response back to the server.
			client.send(response); 
		});

	// Send a message to the server.
	client.send("Hello world\n");

	// Close connection.
	client.close();
}

int main()
{
	// Init.
	PClient client("127.0.0.1", "1444");

	// Connect to the server.
	if (client.connect())
	{
		// Send message to the server.
		client.send("Hello world\n");

		// Close connection to the server.
		client.close();
	}

	// Example for also receiving data.
	Example_For_Receiving();

	getchar();
	return 0;
}