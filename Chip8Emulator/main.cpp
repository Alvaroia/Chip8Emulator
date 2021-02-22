#include <iostream>
#include <fstream>
#include <memory>
#include <array>
#include <string>

#include "cpu.h"
//#include <SFML/Graphics.hpp>

//TODO: 
//1- Keyboard input [X]
//2- Timers
//3- Create sfml clases/ decouple sfml from chip-8 interpreter
void handleKeyboardEvent(const sf::Event& event, cpu& chip8);



int main() {

	cpu chip8;

	chip8.load("keypadTest.ch8");

	// BC_test.ch8 https://slack-files.com/T3CH37TNX-F3RKEUKL4-b05ab4930d
	// c8_test.ch8 https://github.com/Skosulor/c8int/blob/master/test/chip8_test.txt


	while (chip8.window.isOpen()) {
		//Fetch
		uint16_t instr = chip8.fetch();
		//Decode
		chip8.decode(instr);
		//Execute

		sf::Event event;
		while (chip8.window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				chip8.window.close();
			}

			handleKeyboardEvent(event, chip8);
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


void handleKeyboardEvent(const sf::Event& event, cpu& chip8) {

	constexpr std::array<int, 16> keyList = {
		sf::Keyboard::Num1,
		sf::Keyboard::Num2,
		sf::Keyboard::Num3,
		sf::Keyboard::Num4,
		sf::Keyboard::Q,
		sf::Keyboard::W,
		sf::Keyboard::E,
		sf::Keyboard::R,
		sf::Keyboard::A,
		sf::Keyboard::S,
		sf::Keyboard::D,
		sf::Keyboard::F,
		sf::Keyboard::Z,
		sf::Keyboard::X,
		sf::Keyboard::C,
		sf::Keyboard::V
	};

	if (event.type == sf::Event::KeyPressed && std::find(keyList.begin(), keyList.end(), event.key.code) != keyList.end())
	{
		switch (event.key.code) {
		case(sf::Keyboard::Num1):
			chip8.keyCode = 0x1;
			break;
		case(sf::Keyboard::Num2):
			chip8.keyCode = 0x2;
			break;
		case(sf::Keyboard::Num3):
			chip8.keyCode = 0x3;
			break;
		case(sf::Keyboard::Num4):
			chip8.keyCode = 0xC;
			break;
		case(sf::Keyboard::Q):
			chip8.keyCode = 0x4;
			break;
		case(sf::Keyboard::W):
			chip8.keyCode = 0x5;
			break;
		case(sf::Keyboard::E):
			chip8.keyCode = 0x6;
			break;
		case(sf::Keyboard::R):
			chip8.keyCode = 0xD;
			break;
		case(sf::Keyboard::A):
			chip8.keyCode = 0x7;
			break;
		case(sf::Keyboard::S):
			chip8.keyCode = 0x8;
			break;
		case(sf::Keyboard::D):
			chip8.keyCode = 0x9;
			break;
		case(sf::Keyboard::F):
			chip8.keyCode = 0xE;
			break;
		case(sf::Keyboard::Z):
			chip8.keyCode = 0xA;
			break;
		case(sf::Keyboard::X):
			chip8.keyCode = 0x0;
			break;
		case(sf::Keyboard::C):
			chip8.keyCode = 0xB;
			break;
		case(sf::Keyboard::V):
			chip8.keyCode = 0xF;
			break;

		}

		chip8.keyPressed = true;

	}

	if (event.type == sf::Event::KeyReleased && std::find(keyList.begin(), keyList.end(), event.key.code) != keyList.end())
	{
		chip8.keyPressed = false;
		chip8.keyReleased = true;
	}
}