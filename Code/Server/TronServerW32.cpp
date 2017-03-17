#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <SFML\Network.hpp>

constexpr int SERVER_TCP_PORT(53000);
constexpr int SERVER_UDP_PORT(53001);

using TcpClient = sf::TcpSocket;
using TcpClientPtr = std::unique_ptr<TcpClient>;
using TcpClients = std::vector<TcpClientPtr>;

bool bindServerPort(sf::TcpListener&);
void listen(sf::TcpListener&, sf::SocketSelector&, TcpClients&);
void connect(sf::TcpListener&, sf::SocketSelector&, TcpClients&);
void receiveMsg(TcpClients&, sf::SocketSelector&);
void runServer();



int main()
{
	runServer();

	std::cout << "Server is closing" << std::endl;
	return 0;
}



bool bindServerPort(sf::TcpListener& _listener)
{
	if (_listener.listen(SERVER_TCP_PORT) != sf::Socket::Done)
	{
		std::cout << "Could not bind server to port" << std::endl;
		std::cout << "Port: " << SERVER_TCP_PORT << std::endl;
		return false;
	}

	return true;
}



void listen(sf::TcpListener& _tcp_listener, sf::SocketSelector& _socket_selector, TcpClients& _tcp_clients)
{
	while (true)
	{
		if (_socket_selector.wait())
		{
			if (_socket_selector.isReady(_tcp_listener))
			{
				connect(_tcp_listener, _socket_selector, _tcp_clients);
			}
			else
			{
				receiveMsg(_tcp_clients, _socket_selector);
			}
		}

	}
}



void connect(sf::TcpListener& _tcp_listener, sf::SocketSelector& _socket_selector, TcpClients& _tcp_clients)
{
	auto client_ptr = std::make_unique<sf::TcpSocket>();
	auto& client_ref = *client_ptr;

	if (_tcp_listener.accept(client_ref) == sf::Socket::Done)
	{
		std::cout << "Client has connected" << std::endl;

		_socket_selector.add(client_ref);
		_tcp_clients.push_back(std::move(client_ptr));
	}
}



void receiveMsg(TcpClients& _tcp_clients, sf::SocketSelector& _socket_selector)
{
	for (auto& client : _tcp_clients)
	{
		auto& sender_ref = *client;
		if (_socket_selector.isReady(sender_ref))
		{
			sf::Packet packet;
			client->receive(packet);

			std::string message = "NULL";
			packet >> message;

			for (auto& connected_client : _tcp_clients)
			{
				//Spams when disconnecting
				connected_client->send(packet);
			}
			std::cout << message;
		}
	}

}



void runServer()
{
	sf::TcpListener tcp_listener;

	if (!bindServerPort(tcp_listener))
	{
		return;
	}

	sf::SocketSelector socket_selector;
	socket_selector.add(tcp_listener);

	TcpClients tcp_clients;
	return listen(tcp_listener, socket_selector, tcp_clients);
}
