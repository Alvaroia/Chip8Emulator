#include <iostream>
#include <fstream>
#include <memory>
#include <array>


int main() {

	std::array<uint8_t, 4096> memory;
	std::ifstream program("IBM Logo.ch8", std::ios::in | std::ios::binary | std::ios::ate);

	if (!program.is_open()) {
		throw(std::invalid_argument("Error opening ch8 program\n"));
	}

	//File has been open by the end (ios::ate option)
	//tellg return the position in the file (the end in this case which is equal to the file size)
	int size = program.tellg();

	//seekg return the file pointer to the beggining of the file so we can read it as usual.
	program.seekg(0, std::ios::beg);

	program.read(reinterpret_cast<char*>(&memory[0x200]), size);

	int programCounter = 0x200; //Addres 512

	while (true) {
		//Fetch
		//Decode
		//Execute
	}

	return 0;
}