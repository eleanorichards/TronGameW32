#include "TronGame.h"
#include <SFML\Network.hpp>

sf::Packet& operator<<(sf::Packet& packet, const TronGame& game)
{
	return packet;
}

sf::Packet& operator >> (sf::Packet& packet, const TronGame& game)
{
	return packet;
}
