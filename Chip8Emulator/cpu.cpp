#include "cpu.h"
#include <fstream>
#include <algorithm>
#include <iostream>

cpu::cpu()
{

	/// Init screen as black
	std::fill(&screen[0][0], &screen[31][63]+1, false);
	loadFonts();

	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> distrib(0, 255);

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
		switch (N) {
		case 0x0:
			// Clear screen
			//std::cout << "00E0 Clear screen\n";
			std::fill(&screen[0][0], &screen[31][63] + 1, false);
			break;
		case 0xE:
			// Return from subroutine
			//std::cout << "00EE return from subroutine\n";

			PC = stack.back();
			stack.pop_back();
			break;
		}
		break;
	case 0x01:
		//Jump
		//std::cout << "1NNN Jump\n";

		PC = NNN;
		break;
	case 0x02:
		//Calls subroutine
		//std::cout << "2NNN call subroutine\n";

		stack.push_back(PC);
		PC = NNN;
		break;
	case 0x03:
		//std::cout << "3XNN skip if equal\n";

		if (registers[X] == NN) {
			PC += 2;
		}
		break;
	case 0x04:
		//std::cout << "4XNN skip if not equal\n";
		if (registers[X] != NN) {
			PC += 2;
		}
		break;
	case 0x05:
		//std::cout << "5XY0 skip if X Y equal\n";
		if (registers[X] == registers[Y]) {
			PC += 2;
		}
		break;
	case 0x06:
		//6XNN (set register VX)
		//std::cout << "6XNN (set register VX)\n";
		//std::cout << "\tV[" << static_cast<int>(X) << "] = "
			//<< static_cast<int>(NN) << "\n";
		registers[X] = NN;
		break;
	case 0x07:
		//7XNN add value to register vx
		//std::cout << "7XNN add value to register vx\n";

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
		{
			//Right shift
			//std::cout << "8XY6 right shift\n";

			uint8_t* selectedRegister = nullptr;

			if (shiftMode == 0) {
				selectedRegister = &registers[Y];
			}
			else {
				selectedRegister = &registers[X];
			}

			//Check value of shifted bit
			if ((*selectedRegister & 0x01) != 0) {
				//Shifted bit equal to one
				registers[0xF] = 1;
			}
			else {
				registers[0xF] = 0;
			}

			registers[X] = *selectedRegister >> 1;

			break;
		}
		case 0x7:
			//Substract X to Y
			//std::cout << "8XY7 Y - X\n";

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
		{
			//std::cout << "8XYE left shift\n";
			// Left shift

			uint8_t* selectedRegister = nullptr;

			if (shiftMode == 0) {
				selectedRegister = &registers[Y];
			}
			else {
				selectedRegister = &registers[X];
			}

			//Check value of shifted bit
			if ((*selectedRegister & 0x80) != 0) {
				//Shifted bit equal to one
				registers[0xF] = 1;
			}
			else {
				registers[0xF] = 0;
			}

			registers[X] = *selectedRegister << 1;
			break;
		}
		default:
			break;
		}

		break;
	case 0x09:
		//std::cout << "9XY0 skip\n";
		if (registers[X] != registers[Y]) {
			PC += 2;
		}
		break;
	case 0x0A:
		//ANNN set index register I
		//std::cout << "ANNN set index register I\n";

		indexRegister = NNN;
		break;
	case 0x0B:
		// Jump with offset
		//std::cout << "BNNN Jump with offset\n";

		// Disclaimer: Ambiguous instruction
		PC = NNN + registers[0];
		break;
	case 0x0C:
	{
		// generate random number
		//std::cout << "CXNN generated random number\n";
		uint8_t randomNumber = static_cast<uint8_t>(distrib(gen));
		//std::cout << "\tnumber " << static_cast<int>(randomNumber) << "\n";
		//std::cout << "\tNN " << static_cast<int>(NN) << "\n";
		//std::cout << "\tNN " << static_cast<int>(randomNumber & NN) << "\n";


		registers[X] = randomNumber & NN;

		break;
	}
	case 0x0D:
	{
		//DXYN display/draw
		//std::cout << "DXYN display/draw\n";
		/*std::cout << "DRAW " << static_cast<int>(registers[X])
				  << " " << static_cast<int>(registers[Y]) << " "
				  << static_cast<int>(N) << "\n";
*/

		uint8_t x = registers[X] % 64;
		uint8_t y = registers[Y];
		uint8_t* spritePtr = &memory[indexRegister];

		registers[0xF] = 0;

		for (size_t row = 0; row < N; ++row) {

			uint16_t y_pos = y + row;

			// Stop if row > max row
			if (y_pos > 31) {
				break;
			}

			for (size_t column = 0; column < 8; ++column) {

				if (*spritePtr & (1 << (7 - column))) {
					uint16_t x_pos = x + column;

					// If pixel turned off activate VF flag
					if (screen[y_pos][x_pos]) { // Could this be optimized?
						registers[0xF] = 1;
					}

					screen[y_pos][x_pos] = !screen[y_pos][x_pos];

				}
			}
			spritePtr++;
		}


		break;
	}
	case 0x0E:
		// Skip if key
		if (NN == 0x9E) {
			// if key VX pressed
			//std::cout << "EX9E Skip if key\n";
			//std::cout << "\tKey " << registers[X] << "\n";

			if (keyPressed && keyCode == registers[X]) {
				PC = PC + 2;
			}
		}
		else if(NN == 0xA1) {
			//std::cout << "EXA1 Skip if not key\n";
			//std::cout << "\tKey " << registers[X] << "\n";
			// if key VX NOT pressed
			if (!keyPressed || keyCode != registers[X]) {
				PC = PC + 2;
			}
		}

		break;
	case 0x0F:
		switch (NN) {
		case(0x07):
			// V[X] = delayTimer
			//std::cout << "FX07 V[X] = delayTimer\n";
			registers[X] = delayTimer;
			break;
		case(0x15):
			//std::cout << "FX15 delayTimer = V[X]\n";
			delayTimer = registers[X];
			break;
		case(0x18):
			//std::cout << "FX18 soundTimer = V[X]\n";
			soundTimer = registers[X];
			break;
		case(0x1E):
			// Add to index
			//std::cout << "FX1E Add to index\n";

			indexRegister += registers[X];

			if (indexRegister > 0xFFF) {
				registers[0x0F] = 1;
			}
			else {
				registers[0x0F] = 0;
			}

			break;
		case(0x0A):
			// Get key
			//std::cout << "FX0A Get key\n";

			// When key pressed
			if (keyPressed) {
				registers[X] = keyCode;
				keyPressed = false;
			}
			else {
				//while key not pressed
				PC = PC - 2;
			}
			// On the original COSMAC VIP, the key was only registered when it was pressed and then released.

			break;
		case(0x29):
		{
			// Font character
			//std::cout << "FX29 Read font character\n";
			//std::cout << "\tchar " << static_cast<int>(registers[X]) << "\n";
			// The original COSMAC VIP interpreter just took the last nibble of VX and used that as the character.
			uint8_t character = registers[X];

			// Each character occupies 5 bytes
			indexRegister = fontAddr + character * 5;
			break;
		}
		case(0x33):
		{
			//binary coded decimal conversion
			/*std::cout << "FX33 binary coded decimal conversion\n";
			std::cout << "\tregisters[X] " << static_cast<int>(registers[X]) << "\n";
			std::cout << "\tregisters[X](0) " << registers[X]/100 << "\n";
			std::cout << "\tregisters[X](1) " << (registers[X]%100)/10 << "\n";
			std::cout << "\tregisters[X](2) " << registers[X] % 10 << "\n";
*/
			uint8_t number = registers[X];
			memory[indexRegister] = number / 100;
			memory[indexRegister + 1] = (number % 100) / 10;
			memory[indexRegister + 2] = number % 10;
			break;
		}
		case(0x55):
			//Store in memory
			//std::cout << "FX55 store in memory\n";
			//Disclaimer: The original CHIP-8 interpreter for the COSMAC VIP actually incremented the I register while it worked.
			for (size_t reg = 0; reg <= X; ++reg) {
				memory[indexRegister + reg] = registers[reg];
			}

			if (memoryMode == 1) {
				indexRegister += X;
			}
			break;
		case(0x65):
			//Load from memory
			//std::cout << "FX65 Load from memory\n";
			//std::cerr << "\tX = " << static_cast<int>(X) << "\n";

			//Disclaimer: The original CHIP-8 interpreter for the COSMAC VIP actually incremented the I register while it worked.
			for (size_t reg = 0; reg <= X; ++reg) {
				registers[reg] = memory[indexRegister + reg];
				//std::cerr << "\t" << static_cast<int>(memory[indexRegister + reg]) << "\n";
			}

			if (memoryMode == 1) {
				indexRegister += X;
			}
			break;
		default: 
			break;
		}
		break;

	default:
		//halt?
		break;
	}
	 

}


void cpu::loadFonts() {

	constexpr std::array<uint8_t, 5 * 16> fonts = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	std::copy(fonts.begin(), fonts.end(), &memory[fontAddr]);
}

double cpu::updateTimers(const double elapsedTime) {

	constexpr int ticPeriod{ 16 }; //60 Hz -> ~16 ms period

	int numberTics = std::floor(elapsedTime / ticPeriod);

	double remainingTime = static_cast<int>(elapsedTime) % ticPeriod
						+ elapsedTime - static_cast<int>(elapsedTime);

	soundTimer = std::max(0, soundTimer - numberTics);
	delayTimer = std::max(0, delayTimer - numberTics);

	return remainingTime;
}