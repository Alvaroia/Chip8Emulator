#include "cpu.h"
#include <fstream>
#include <algorithm>
#include <iostream>

cpu::cpu() :
	window(sf::VideoMode(64 * 4, 32 * 4, 1), "CHIP-8") //Sacar esto fuera
{
	window.setSize({ 64 * 16 , 32 * 16 });

	/// Init screen as black
	screen.fill(false);
}

int cpu::load(const std::string& file) {
	std::ifstream program(file, std::ios::in | std::ios::binary | std::ios::ate);

	if (!program.is_open()) {
		throw(std::invalid_argument("Error opening ch8 program\n"));
	}

	//File has been open by the end (ios::ate option)
	//tellg return the position in the file (the end in this case which is equal to the file size)
	size_t size = program.tellg();

	//seekg return the file pointer to the beggining of the file so we can read it as usual.
	program.seekg(0, std::ios::beg);

	program.read(reinterpret_cast<char*>(&memory[0x200]), size);

	return 0;
}

uint16_t cpu::fetch() {

	uint16_t nextInstruction = static_cast<uint16_t>(memory[PC]) << 8 | memory[PC + 1];

	//set program counter to next instruction
	PC = std::min(PC + 2, 4096 - 2);


	return nextInstruction;
}

void cpu::decode(const uint16_t instruction) {

	/// First 4 bits opcode
	uint8_t opcode = instruction >> 12;
	uint8_t X = instruction >> 8 & 0x0F;
	uint8_t Y = instruction >> 4 & 0x0F;
	uint8_t N = instruction & 0x000F;
	uint8_t NN = instruction & 0x00FF;
	uint16_t NNN = instruction & 0x0FFF;

	switch (opcode) {
	case 0x00:
		//Clear screen or return from subroutine
		std::cout << "Clear screen\n";
		screen.fill(false);
		break;
	case 0x01:
		//Jump
		PC = NNN;
		break;
	case 0x02:
		//Calls subroutine
		break;
	case 0x03:
		if (registers[X] == NN) {
			PC += 2;
		}
		break;
	case 0x04:
		break;
	case 0x05:
		if (registers[X] == registers[Y]) {
			PC += 2;
		}
		break;
	case 0x06:
		//6XNN (set register VX)
		std::cout << "6XNN (set register VX)\n";
		registers[X] = NN;
		break;
	case 0x07:
		//7XNN add value to register vx
		std::cout << "7XNN add value to register vx\n";

		registers[X] += NN;
		break;
	case 0x08:

		switch (N) {
		case 0x0:
			registers[X] = registers[Y];
			break;
		case 0x1:
			registers[X] |= registers[Y];
			break;
		case 0x2:
			registers[X] &= registers[Y];
			break;
		case 0x3:
			registers[X] ^= registers[Y];
			break;
		case 0x4:
			//Add
			registers[X] += registers[Y];

			/// Check overflow
			if (registers[X] < registers[Y]) {
				registers[0xF] = 1;
			}
			else {
				registers[0xF] = 0;
			}
			break;
		case 0x5:
			//Substract Y to X
			//Flag to one if no underflow
			if (registers[X] > registers[Y]) {
				registers[0xF] = 1;
			}
			else {
				registers[0xF] = 0;
			}

			registers[X] -= registers[Y];


			break;
		case 0x6:
			//Right shift

			//Check value of shifted bit
			if (registers[Y] & 0x01 != 0) {
				//Shifted bit equal to one
				registers[0xF] = 1;
			}
			else {
				registers[0xF] = 0;
			}

			registers[X] = registers[Y] >> 1;

			break;
		case 0x7:
			//Substract X to Y
			//Flag to one if no underflow
			if (registers[Y] > registers[X]) {
				registers[0xF] = 1;
			}
			else {
				registers[0xF] = 0;
			}

			registers[X] = registers[Y] - registers[X];

			break;
		case 0xE:
			//Check value of shifted bit
			if (registers[Y] & 0x80 != 0) {
				//Shifted bit equal to one
				registers[0xF] = 1;
			}
			else {
				registers[0xF] = 0;
			}

			registers[X] = registers[Y] << 1;
			break;
		default:
			break;
		}

		break;
	case 0x09:
		if (registers[X] != registers[Y]) {
			PC += 2;
		}
		break;
	case 0x0A:
		//ANNN set index register I
		std::cout << "ANNN set index register I\n";

		indexRegister = NNN;
		break;
	case 0x0B:
		PC = NNN + registers[0];
		break;
	case 0x0C:
	{
		uint8_t randomNumber = 1; //TODO
		registers[X] = randomNumber & NN;

		break;
	}
	case 0x0D:
	{
		//DXYN display/draw
		std::cout << "DXYN display/draw\n";
		std::cout << "DRAW " << static_cast<int>(registers[X])
			<< " " << static_cast<int>(registers[Y]) << " "
			<< static_cast<int>(N) << "\n";


		uint8_t x = registers[X] % 64;
		uint8_t y = registers[Y];
		uint8_t* spritePtr = &memory[indexRegister];


		for (size_t row = 0; row < N; ++row) {
			for (size_t column = 0; column < 8; ++column) {

				if (*spritePtr & (1 << (7 - column))) {
					uint16_t x_pos = x + column;
					uint16_t y_pos = y + row;
					screen[y_pos * 64 + x_pos] = !screen[y_pos * 64 + x_pos];

				}
			}
			spritePtr++;
		}


		break;
	}
	case 0x0E:
		break;
	case 0x0F:
		break;

	default:
		//halt?
		break;
	}





}
