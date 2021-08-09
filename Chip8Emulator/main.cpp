#include "cpu.h"
#include "gui.h"
#include <SFML/Graphics.hpp> //This order is important https://en.sfml-dev.org/forums/index.php?topic=25059.0
//#include <windows.h> // WinApi header 


#include <iostream>
#include <fstream>
#include<cmath>
#include <memory>
#include <array>
#include <string>
#include <chrono>
#include <thread>
#include <numeric>


//TODO: 
//1- Keyboard input [X]
//2- Timers [X]
//3- Create sfml clases/ decouple sfml from chip-8 interpreter
//4- implement observer to handle events from GUI to chip and from chip to GUI
//WARNINGS:
//1- draw instruction: clipping de las zonas que salen de la imagen?
//2- keyboards scancodes?
void handleKeyboardEvent(const sf::Event& event, cpu& chip8);
void makeBeep(const uint8_t delayTimer);



int main() {

	cpu chip8;

	Gui<32, 64> screen(&chip8.screen, 16, std::string("CHIP-8"));


	chip8.load("./ROMS/DelayTimerTest.ch8");

	auto previous_cpu_time = std::chrono::high_resolution_clock::now();
	double remainingTime = 0.0;
	bool programEnd = false;

	// BC_test.ch8 https://slack-files.com/T3CH37TNX-F3RKEUKL4-b05ab4930d [X]
	// c8_test.ch8 https://github.com/Skosulor/c8int/blob/master/test/chip8_test.txt [X]
	// test_opcode.ch8 [X]
	// keypadTest.ch8 [X]
	// DelayTimerTest [?]
	// DivisionTest.ch8 [?]
	// HiresTest.ch8 (HI-RESolution shouldn't work)
	// Pong.ch8 [X]
	// SpaceFlight [X]

	//Sound processing moved to another thread to avoid delay in main loop
	//std::thread beepThread(
	//	[&chip8, &programEnd]() {
	//	while (!programEnd) {
	//		if (static_cast<int>(chip8.soundTimer) != 0) {
	//			Beep(523, 500); // 523 hertz (C5) for 500 milliseconds
	//		}
	//	}
	//});


	std::array<double, 1000> elapsedTimes;
	std::fill(elapsedTimes.begin(), elapsedTimes.end(), 0);

	int frame_cont{ 0 };
	
	while (!programEnd) {

		// Update timers
		using namespace std::chrono;
		auto current_cpu_time = high_resolution_clock::now();

		auto elapsedTime = duration_cast<microseconds>(current_cpu_time - previous_cpu_time);

		previous_cpu_time = current_cpu_time;

		// FPS counter using a sliding window
		elapsedTimes[frame_cont] = elapsedTime.count();
		
		if (frame_cont < elapsedTimes.size() - 1)
		{
			++frame_cont;
		}
		else
		{
			std::cerr << "FPS "	
				<< 1.0 / (std::accumulate(elapsedTimes.begin(), elapsedTimes.end(), 0.0) / elapsedTimes.size() / 1e6) 
				<< "\n";
			frame_cont = 0;
		}


		remainingTime = chip8.updateTimers(elapsedTime.count() + remainingTime);


		//Fetch
		uint16_t instr = chip8.fetch();

		//Decode + Execute
		chip8.decode(instr);

		//Handle window events
		std::tie(programEnd, chip8.keyPressed, chip8.keyReleased, chip8.keyCode) = screen.handleEvents();

		//Draw screen
		screen.draw();

		const int minInstructionTime = std::round(1.0 / 1200 * 1e6); //us

		const auto minInstructionTimeMS = std::chrono::microseconds(minInstructionTime);

		auto current_cpu_time2 = high_resolution_clock::now();


		auto loopTime = duration_cast<microseconds>(current_cpu_time2 - current_cpu_time);


		if(loopTime < minInstructionTimeMS)
		{
			std::this_thread::sleep_for(minInstructionTimeMS - loopTime);
		}

	}

	//beepThread.join();

	return 0;
}
