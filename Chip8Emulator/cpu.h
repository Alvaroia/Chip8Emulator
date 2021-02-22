#pragma once
#include <stdint.h>
#include <array>
#include <string>
#include <random>


//TODO: desacoplar esto a otro lado
#include <SFML/Graphics.hpp>


class cpu
{
public:
	cpu();
	cpu(const cpu& copy) = default;
	~cpu() = default;

	/// Load program
	int load(const std::string& file);

	/// Fetch instruction
	uint16_t fetch();

	/// Decode instruction
	void decode(const uint16_t instruction);

	/// Execute instruction

	uint16_t stackPtr = 0x0;
	uint16_t indexRegister = 0x0;
	uint8_t delayTimer = 0x0;
	uint8_t soundTimer = 0x0;


	/// 8 bits Registers
	std::array<uint8_t, 16> registers;

	//TODO: desacoplar esto a otro lado
	sf::RenderWindow window;

	/// Black 0, white 1
	std::array<bool, 64*32> screen;

	/// keyCode of the last key pressed
	/// Posible values from 0x0 to 0xF
	uint8_t keyCode = 0x0;

	bool keyPressed = false;
	bool keyReleased = false;


private:
	std::array<uint8_t, 4096> memory;

	std::vector<uint16_t> stack;
	
	uint16_t PC = 0x200; //Addres 512

	uint16_t fontAddr = 0x50;
	

	/// Load fonts into memory
	void loadFonts();

	std::random_device rd;  //Will be used to obtain a seed for the random number engine

	std::mt19937 gen; //Standard mersenne_twister_engine seeded with rd()

	std::uniform_int_distribution<> distrib;

	int shiftMode = 1;
	int memoryMode = 0;




};

