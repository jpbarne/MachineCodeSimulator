// Jason Barnes & Mitchell Bowser
// CPSC 3300 - Prof. Mark Smotherman
// Project 2 - DUE: March 15, 2018
//
// "Simulate Eckert's Microprogrammed Machine"
//
// COMPILE: g++ -Wall -std=c++11 main.cpp


#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>
#include <string>
#include <cmath>

//********Globals**********//
std::map <int, std::string> ops
	{{0, "ip"}, {1, "lp"}, {2, "ep"}, {3, "lm"}, {4, "r"}, {5, "w"},
	 {6, "ld"}, {7, "ed"}, {8, "li"}, {9, "ei"}, {10, "la"},
	 {11, "ea"}, {12, "a"}, {13, "s"}, {14, "eu"}, {15, "lb"},
	 {16, "cd"}, {17, "map"}, {18, "hlt"}};

int pc = 0, mar = 0, ir = 0, mdr = 0, acc = 0, b = 0, alu = 0, halt = 0;

std::string instructs[8] = {
	"lda", "sta", "add", "sub", "mba", "jmp", "jn", "hlt"
};

int RAM[256];
int wordCnt;
int addrROM[16];
int ctrlROM[32];

bool signals[19];
int crja;
int bus;
int microCntr = 0;
int oldUCnt = 0;

//******Functions*******//

//load and read files
void load_ram();
void load_rom();
void load_uprog();

//print while executing
void print_cROM(int word);
bool execute();
void print_registers();
void set_signals(int word);
void print_signals();
void print_ram();

int main(int argc, char const *argv[]) {
	load_ram();
	load_rom();
	load_uprog();

	//print initial register values
	pc = 0; mar = 0; mdr = 0; acc = 0; alu = 0; b = 0; ir = 0;
	std::cout << std::endl << "initial register values" << std::endl;
	std::cout << " pc mar mdr acc alu   b   ir" << std::endl;
	std::cout << "  " << pc << "   " << mar << "   " <<  mdr << "   "
				 << acc << "   " << alu << "   " << b << "   " << ir << std::endl;

	//execute microinstructions
	std::cout << std::endl
				 << "control signals and register values after each microinstruction"
				 << std::endl;

	std::cout << "                     mh" << std::endl
				 << "   ilelrwleleleasel cal" << std::endl
				 << "uc pppm  ddiiaa  ub dpt ja pc mar mdr acc alu   b  ir"
				 << std::endl;

	bool running = true;
	while(running){ // while no halt signal
		set_signals(microCntr);
		running = execute();
		print_cROM(oldUCnt);
		print_registers();
		print_signals();
	}
	print_ram(); //print final state of RAM

	return 0;
}

void load_ram() {
	std::ifstream ram("ram.txt");

	//print and store RAM from ram file
	std::cout << "Contents of RAM memory" << std::endl
	          << "addr value" << std::endl;
	int i = 0, value;
	while (ram >> std::hex >> value) {
		RAM[i] = value; //value & 0xfff;
		std::cout << std::setw(2) << std::setfill(' ') << i++ << std::hex
							<< ": " << std::setfill('0') << std::setw(3)
		          << value << std::endl;

		if(i >= 256) {
			std::cout << "RAM overflow" << std::endl;
			exit(1);
		}
	}
	wordCnt = i;
	for(; i < 256; i++)	RAM[i] = 0;

	ram.close();
}

/**** Open ADDR.TXT and print contents****/
void load_rom() {
	std::ifstream addr("addr.txt");

	//print and store address ROM from addr file
	std::cout << std::endl << "Contents of address ROM" << std::endl
	          << "opc addr" << std::endl;
	int i = 0, value;
	while (addr >> std::hex >> value) {
		addrROM[i] = value & 0x1f;
		std::cout << i++ << ": " << std::setfill('0') << std::setw(2)
		          << std::hex << value << std::endl;
	}
	std::cout << std::endl;
	addr.close();
}

/**** Open UPROG.TXT, print contents, and set ctrlROM array****/
void load_uprog() {
	std::ifstream uprog("uprog.txt");

	//print control ROM
	std::cout << "Contents of control ROM with active signals identified"
	 					<< std::endl << "addr contents" << std::endl;
	int i = 0, value;
	while (uprog >> std::hex >> value) {
		ctrlROM[i] = value & 0xffffff;

		//Print out uprog contents
		std::cout << std::setfill(' ') << std::setw(2) << i++ << ": "
		<< std::setfill('0') << std::setw(6) << std::hex << value << " ";

		//decode line and determine signal codes, then print
		for(int code = 0; code < 19; code++)
		{
			int mask = (std::pow(2, 23 - code));
			int result = mask & value;

			if(result) std::cout << ops[code] << " ";
			else {
				if (code == 4 || code == 5 || code == 12 || code == 13)
					std::cout << "  ";
				else if (code == 17 || code == 18)
					std::cout << "    ";
				else
					std::cout << "   ";
			}
		}

		int crja = value & 31;
		std::cout << " crja=" << std::setfill('0') << std::setw(2) << std::hex
							<< crja	<< std::endl;
	}
	uprog.close();
}

/***Sets control signals for current word so that execute can run ***/
void set_signals(int word) {
	int value = ctrlROM[word];

	//for each code signal
	for(int code = 0; code < 19; code++)
	{
		int mask = (std::pow(2, 23 - code));
		int result = mask & value;
		if(result) signals[code] = true;
		else signals[code] = false;
	}
	crja = value & 31;
}

/**** prints out signals from previous instruction ****/
void print_cROM(int word) {
		int value = ctrlROM[word];

		//uc - microcounter
		std::cout << std::hex <<std::setfill('0') << std::setw(2) << microCntr << " ";

		for(int code = 0; code < 19; code++)
		{
			int mask = (std::pow(2, 23 - code));
			int result = mask & value;

			if(result) std::cout << 1;
			else std::cout << 0;

			if(code == 15 || code == 18) std::cout << " ";
		}
		//crja
		std::cout << std::setfill('0') << std::setw(2) << std::hex << (value & 31);
}

//performs all necessary actions based on signals - updates microcounter
bool execute(){
	//Enables signals
	if(signals[2])//EP
		bus = pc;
	if(signals[7])//ED
		bus = mdr;
	if(signals[9])//EI
		bus = ir & 0xff;
	if(signals[11])//EA
		bus = acc;
	if(signals[14])//EU
		bus = alu;

	//Loads signals
	if(signals[1])//LP
		pc = bus & 0xff;
	if(signals[3])//LM
		mar = bus &  0xff;
	if(signals[6])//LD
		mdr = bus & 0xfff;
	if(signals[8])//LI
		ir = bus;
	if(signals[10])//LA
		acc = bus;
	if(signals[15])//LB
		b = bus;

	//Read Write signals
	if(signals[4])//R
		mdr = RAM[mar];
	if(signals[5])//W
		RAM[mar] = mdr;

	//ALU signals
	if(signals[12])//A
		alu = (acc + b) & 0xfff;
	if(signals[13])//S
		alu = (acc - b) & 0xfff;

	//PC
	if(signals[0])//IP
		pc++;

		//For testing purposes
	oldUCnt = microCntr;

	if(signals[17]) // MAP
		microCntr = addrROM[mdr >> 8];// 100];
	if(!signals[17] && !signals[16])
		microCntr = crja;
	if(signals[16])
		microCntr = crja;

	return (signals[18])? 0: 1; //0 if halt signal
}

/*** Print PC, MAR, MDR, ACC, ALU, B, and IR ***/
void print_registers(){
	std::cout << "  " << pc << std::setfill(' ') << std::setw(4) << mar
						<< std::setw(4) << mdr << std::setw(4) << acc << std::setw(4) << alu
					  << std::setw(4) << b << std::setw(4) << ir;
}

/*** print operations code if active ***/
void print_signals(){
	for(int code = 0; code < 19; code++)
		if(signals[code]) std::cout << " " << ops[code];

	//new line for finished instrution
	if(microCntr == 0) std::cout << std::endl << std::endl;
	else std::cout << std::endl;
}

/*** Print final contents of RAM ***/
void print_ram(){
	std::cout << "Contents of RAM memory" << std::endl
						<< "addr value" << std::endl;

	for(int i = 0; i < 23; i++) {
		if (RAM[i]) std::cout << std::setfill(' ') << std::setw(2) << i << std::hex
													<< ": " << std::setfill('0') << std::setw(3) << RAM[i]
													<< std::endl;
	}
}
