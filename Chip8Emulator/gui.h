#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <tuple>
#include <memory>


template<int rows, int cols>
class Gui
{
public:
	Gui(std::array<std::array<bool, cols>, rows >* screen_,
		int scale,
		std::string tabName) :
		window(sf::VideoMode(cols * 4, rows * 4, 1), "CHIP-8")
	{
		const sf::Vector2u windowSize(cols * scale, rows * scale);
		window.setSize(windowSize);
		screen = screen_;

	}

	void draw()
	{
		window.clear();

		drawScreen();

		drawGrid();

		window.display();
	}

	std::tuple<bool, bool, bool, uint8_t>
	handleEvents() {

		sf::Event event;
		bool programEnd{ false };
		bool keyPressed{ false };
		bool keyReleased{ false };
		uint8_t keyCode{ 0xFF };

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				programEnd = true;
				window.close();
			}


			if (event.type == sf::Event::KeyPressed
				&&
				std::find(keyList.begin(), keyList.end(), event.key.code) != keyList.end())
			{
				switch (event.key.code) {
				case(sf::Keyboard::Num1):
					keyCode = 0x1;
					break;
				case(sf::Keyboard::Num2):
					keyCode = 0x2;
					break;
				case(sf::Keyboard::Num3):
					keyCode = 0x3;
					break;
				case(sf::Keyboard::Num4):
					keyCode = 0xC;
					break;
				case(sf::Keyboard::Q):
					keyCode = 0x4;
					break;
				case(sf::Keyboard::W):
					keyCode = 0x5;
					break;
				case(sf::Keyboard::E):
					keyCode = 0x6;
					break;
				case(sf::Keyboard::R):
					keyCode = 0xD;
					break;
				case(sf::Keyboard::A):
					keyCode = 0x7;
					break;
				case(sf::Keyboard::S):
					keyCode = 0x8;
					break;
				case(sf::Keyboard::D):
					keyCode = 0x9;
					break;
				case(sf::Keyboard::F):
					keyCode = 0xE;
					break;
				case(sf::Keyboard::Z):
					keyCode = 0xA;
					break;
				case(sf::Keyboard::X):
					keyCode = 0x0;
					break;
				case(sf::Keyboard::C):
					keyCode = 0xB;
					break;
				case(sf::Keyboard::V):
					keyCode = 0xF;
					break;

				}

				keyPressed = true;

			}

			if (event.type == sf::Event::KeyReleased && std::find(keyList.begin(), keyList.end(), event.key.code) != keyList.end())
			{
				keyPressed = false;
				keyReleased = true;
			}

		}
		return std::make_tuple(programEnd, keyPressed, keyReleased, keyCode);
		//Or send notification to subscribers

	}



private:

	sf::RenderWindow window;

	std::array<std::array<bool, cols>, rows>* screen;

	constexpr static std::array<int, 16> keyList = {
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
		sf::Keyboard::V};

	void drawScreen()
	{

		for (size_t row = 0; row < rows; ++row)
		{
			for (size_t col = 0; col < cols; ++col)
			{
				if ((*screen)[row][col])
				{
					sf::RectangleShape pixel(sf::Vector2f(4, 4));
					pixel.setPosition(std::floor(col * 4), std::floor(row * 4));
					pixel.setFillColor(sf::Color::White);
					window.draw(pixel);
				}
			}
		}

	}

	void drawGrid()
	{
		for (size_t col = 0; col < cols * 4; col += 4) {
			sf::Vertex line[] =
			{
				sf::Vertex(sf::Vector2f(col, 0), sf::Color::Black),
				sf::Vertex(sf::Vector2f(col, rows * 4), sf::Color::Black)
			};

			window.draw(line, 2, sf::Lines);
		}

		for (size_t row = 0; row < rows * 4; row += 4) {
			sf::Vertex line[] =
			{
				sf::Vertex(sf::Vector2f(0, row), sf::Color::Black),
				sf::Vertex(sf::Vector2f(cols * 4, row), sf::Color::Black)
			};

			window.draw(line, 2, sf::Lines);
		}
	}

};

