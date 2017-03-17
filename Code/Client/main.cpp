#include <SFML/Graphics.hpp>

#include <future>
#include <iostream>
#include <string>

#include <dinput.h>

#include <SFML\Network.hpp>

using TcpClient = sf::TcpSocket;
using TcpClientPtr = std::unique_ptr<TcpClient>;
using TcpClients = std::vector<TcpClientPtr>;
const sf::IpAddress SERVER_IP("127.0.0.1");
constexpr int SERVER_TCP_PORT(53000);

void client();
bool connect(TcpClient& _client);
void input(TcpClient& _client);

int main()
{
	std::thread clientThread(client);
	//std::thread thread(PollInput);

	sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(shape);
		window.display();
	}

	return 0;
}


void PollInput()
{
	std::atomic<char> input;
	char textInput;

	while (true)
	{
		std::cin >> textInput;
		input.store(textInput);
	}
}

void client()
{
	TcpClient socket;
	if (!connect(socket))
	{
		return;
	}

	auto handle = std::async(std::launch::async, [&]
	{
		// keep track of the socket status
		sf::Socket::Status status;
		do
		{
			sf::Packet packet;
			status = socket.receive(packet);
			if (status == sf::Socket::Done)
			{
				std::string message = "NULL";
				
				std::cin >> message;
				
				packet >> message;

				std::cout << message << std::endl;
			}
		} while (status != sf::Socket::Disconnected);
	});

	return input(socket);
}



bool connect(TcpClient& _client)
{
	sf::Socket::Status status = _client.connect(SERVER_IP, SERVER_TCP_PORT);

	if (status != sf::Socket::Done)
	{
		return false;
	}

	std::cout << "Connected to server: " << SERVER_IP << std::endl;

	return true;
}



void input(TcpClient& _client)
{
	while (true)
	{
		auto& sender_ref = _client;

		sf::Packet packet;
		std::string message = "NULL";

		std::cin >> message;
		packet << message;

		_client.send(packet);
	}
}
