#include "cpu.h"
#include "Gui.h"
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
	
	while (screen.window.isOpen()) {

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


		// Handle events
		sf::Event event;
		while (screen.window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				programEnd = true;
				screen.window.close();
			}

			handleKeyboardEvent(event, chip8);
		}

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

	if (event.type == sf::Event::KeyPressed 
		&& 
		std::find(keyList.begin(), keyList.end(), event.key.code) != keyList.end())
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
