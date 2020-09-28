#ifndef PCLIENT_H
#define PCLIENT_H

#include <string>
#include <functional>
#include <thread>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <atomic>

/* Simple TCP Client class. 
*  This class aims to be give you basic TCP Client functionality while keep things simple and easy for you.
*  You only have to give an IP Address and Port to start a connection.
*  If you also want to receive data you will have to set an callback function in the constructor or the connect function call.
\ ======================================================================================================================== /
*  Author:  Deniz Sonsal @ github.com/deniz617 
*  Date:    28/09/2020 
*  Contact: deniz.frosty@gmail.com
*/
class PClient
{
private:
	std::atomic_bool stopThread;

	void recv(PClient* t);
	void spawn_recv_thread();
	void clean_recv_thread();

	std::thread* recv_thread;
	std::string ip;
	std::string port;
	std::function<void(std::string response)> callback{ nullptr };

	boost::asio::io_service io_service;
	boost::asio::ip::tcp::socket socket;
public:
	/* Creates a tcp client that can connect to any ipv4 tcp server.
	* Arguments: IP Address, Port, Callback function(optional).
	* Foreach packet received your callback function will be called with the packet as an argument.
	*/
	PClient(std::string ip, std::string port, std::function<void(std::string response)> callback = nullptr) : socket(io_service){
		this->ip = ip;
		this->port = port;
		this->callback = callback;
	}

	~PClient() {
		clean_recv_thread();
	}

	/* Attempts to connect.
	*  Arguments: IP Address(optional), Port(optional), Callback function(optional).
	*  If you don't pass arguments it will use the arguments you gave to the constructor.
	*  The arguemtsn from the constructor will be overwritten if you pass arguments to this function.
	*/
	bool connect(std::string ip = "", std::string port = "", std::function<void(std::string response)> callback = nullptr);

	/* Terminate connection.
	*/
	void close();

	/* Sends data to the server.
	*  Arguments: Packet.
	*  Returns length of sent bytes.
	*/
	size_t send(std::string packet);
};


#endif