#pragma once
#include <stdint.h>
#include <array>
#include <string>

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


	/// 8 bits Registers
	std::array<uint8_t, 16> registers;

	//TODO: desacoplar esto a otro lado
	sf::RenderWindow window;

	/// Black 0, white 1
	std::array<bool, 64*32> screen;



private:
	std::array<uint8_t, 4096> memory;
	uint16_t PC = 0x200; //Addres 512




};

