#include <iostream>
#include <fstream>
#include <memory>
#include <array>
#include <string>

#include "cpu.h"
//#include <SFML/Graphics.hpp>



int main() {

	cpu chip8;

	chip8.load("IBM Logo.ch8");

	uint8_t delayTimer = 0x0;
	uint8_t soundTimer = 0x0;

	while (chip8.window.isOpen()) {
		//Fetch
		uint16_t instr = chip8.fetch();
		//Decode
		chip8.decode(instr);
		//Execute

		sf::Event event;
		while (chip8.window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				chip8.window.close();
		}

		chip8.window.clear();

		for (size_t row = 0; row < 32; ++row) {
			for (size_t col = 0; col < 64; ++col) {
				if (chip8.screen[row * 64 + col]) {
					sf::RectangleShape pixel(sf::Vector2f(4, 4));
					pixel.setPosition(std::floor(col * 4), std::floor(row * 4));
					pixel.setFillColor(sf::Color::White);
					chip8.window.draw(pixel);
				}
			}
		}


		for (size_t col = 0; col < 64*4; col+=4) {
			sf::Vertex line[] =
			{
				sf::Vertex(sf::Vector2f(col, 0), sf::Color::Black),
				sf::Vertex(sf::Vector2f(col, 32*4), sf::Color::Black)
			};

			chip8.window.draw(line, 2, sf::Lines);
		}

		for (size_t row = 0; row < 32 * 4; row += 4) {
			sf::Vertex line[] =
			{
				sf::Vertex(sf::Vector2f(0, row), sf::Color::Black),
				sf::Vertex(sf::Vector2f(64*4, row), sf::Color::Black)
			};

			chip8.window.draw(line, 2, sf::Lines);
		}



		chip8.window.display();
	}

	return 0;
}
