#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
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
		screen = screen_ ;

	}

	void draw() 
	{
		window.clear();

		drawScreen();

		drawGrid();

		window.display();
	}

	sf::RenderWindow window;

private:

	std::array<std::array<bool, cols>, rows>* screen;
	
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

	//void handleKeyboardEvent(); // Observer pattern?

};
