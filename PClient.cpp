#include "PClient.h"
#include <iostream>

bool PClient::connect(std::string ip, std::string port, std::function<void(std::string response)> callback)
{
	if (!ip.empty())
		this->ip = ip;
	if (!port.empty())
		this->port = port;
	if (callback != nullptr)
		this->callback = callback;

	// Code for connection attempt.
	boost::asio::ip::tcp::resolver resolver(io_service);
	boost::asio::ip::tcp::resolver::query query(this->ip, this->port);
	boost::asio::ip::tcp::resolver::iterator it_end;
	boost::system::error_code err = boost::asio::error::host_not_found;

	for(boost::asio::ip::tcp::resolver::iterator it_endpoint = resolver.resolve(query, err); !err && it_endpoint != it_end;)
	{ 
		socket.close();
		socket.connect(*it_endpoint++, err);
	}

	// TODO: A better check if connection was successful?
	if (!err && socket.is_open())
	{
		clean_recv_thread();
		spawn_recv_thread();

		return true;
	}

	return false;
}

void PClient::close()
{
	socket.close();
	clean_recv_thread();
}

size_t PClient::send(std::string packet)
{
	// Code to send data.
	boost::system::error_code err;
	size_t len = socket.send(boost::asio::buffer(packet), 0, err);
	if (err)
	{
		std::cout << "[PClient][send] Exception: " << err.message() << std::endl;
	}
	return len;
}

void PClient::recv(PClient* t)
{
	std::string received_packet;
	boost::array<char, 512> buf;
	boost::system::error_code err;
	while (!t->stopThread.load())
	{
		// Code for receiving data.
		size_t len = (t)->socket.read_some(boost::asio::buffer(buf), err);
		received_packet = std::string(buf.data(), len);

		// TODO: Better check if connection got terminated/lost?
		if (!err)
		{
			// Call the callback function.
			if((t)->callback != nullptr)
				(t)->callback(received_packet);
		}
		else
			break;
	}
}

void PClient::spawn_recv_thread()
{
	stopThread.store(false);
	recv_thread = new std::thread([&]() { this->recv(this); });
}

void PClient::clean_recv_thread()
{
	if (this->recv_thread != nullptr)
	{
		stopThread.store(true);
		try
		{
			this->recv_thread->join();
			delete this->recv_thread;
		}
		catch (std::exception& e)
		{
			std::cout << "[Clean][RecvThread] Exception: " << e.what() << std::endl;
		}
		this->recv_thread = nullptr;
	}
}
