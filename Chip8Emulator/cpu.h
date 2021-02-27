#pragma once
#include <stdint.h>
#include <array>
#include <string>
#include <random>


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

	/// Timer update (60Hz)
	double updateTimers(const double elapsedTime);

	/// Execute instruction


	uint8_t delayTimer = 0x00;

	uint8_t soundTimer = 0x00;

	/// Black 0, white 1
	std::array<std::array<bool, 64>, 32> screen;

	/// keyCode of the last key pressed
	/// Posible values from 0x0 to 0xF
	uint8_t keyCode = 0x0;

	bool keyPressed = false;

	bool keyReleased = false;


private:
	/// CPU RAM
	std::array<uint8_t, 4096> memory;

	/// CPU Stack: used only for subroutines
	std::vector<uint16_t> stack;

	/// 8 bits Registers
	std::array<uint8_t, 16> registers;

	/// Stack pointer (unused)
	uint16_t stackPtr = 0x0;

	/// Index pointer: Used to read/write in memory
	uint16_t indexRegister = 0x0;

	/// Program counter
	/// ROM space start at address 512 (0x200)
	uint16_t PC = 0x200;

	/// Address in which fonts are stored
	uint16_t fontAddr = 0x50;


	/// Load fonts into memory
	void loadFonts();


	/// Variables required for random number generator
	std::random_device rd;  //Will be used to obtain a seed for the random number engine

	std::mt19937 gen; //Standard mersenne_twister_engine seeded with rd()

	std::uniform_int_distribution<> distrib;


	/// Compatibility flags
	int shiftMode = 1;

	int memoryMode = 0;

};

