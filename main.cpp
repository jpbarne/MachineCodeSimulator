#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>
#include <string>
#include <cmath>

//********Globals**********//
std::map <int, std::string> ops
	{{0, "IP"}, {1, "LP"}, {2, "EP"}, {3, "LM"}, {4, "R"}, {5, "W"},
	 {6, "LD"}, {7, "ED"}, {8, "LI"}, {9, "EI"}, {10, "LA"},
	 {11, "EA"}, {12, "A"}, {13, "S"}, {14, "EU"}, {15, "LB"},
	 {16, "CD"}, {17, "MAP"}, {18, "HLT"}};

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

//
// //instructrion fetch routine
// void fetch() {
// 	mar = pc;
// 	mdr = RAM[mar];
// 	ir = mdr;
// 	pc++;
// }
//
// //set of instruction execution routines
// void inv() {
// 	std::cout << "invalid opcode" << std::endl;
// 	exit(1);
// }
//
// void lda() {
// 	mar = ir & 0xff;
// 	mdr = RAM[mar];
// 	acc = mdr;
// }
//
// void sta() {
// 	mar = ir & 0xff;
// 	mdr = acc;
// 	RAM[mar] = mdr;
// }
//
// void add() {
// 	alu = (acc + b) & 0xfff;
// 	acc = alu;
// }
//
// void sub() {
// 	alu = (acc - b) & 0xfff;
// 	acc = alu;
// }
//
// void mba() {
// 	b = acc;
// }
//
// void jmp() {
// 	pc = ir & 0xff;
// }
//
// void jneg() {
// 	if (acc >> 11) {
// 		pc = ir & 0xff;
// 	}
// }
//
// void hlt() {
// 	halt = 1;
// }

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


	//The i = 4 used to limit microCntr below is arbitrary, just to stop infinite
	//loops for now - does first 4 cycles
	//int i = 0;
	bool running = true;
	while(running){//!signals[18]){
		set_signals(microCntr);
		running = execute();
		print_cROM(oldUCnt);
		print_registers();
		print_signals();

		//i++;
	}
	print_ram();

	return 0;
}

void load_ram() {
	std::ifstream ram("ram.txt");

	//print ram
	std::cout << "Contents of RAM memory" << std::endl
	          << "addr value" << std::endl;
	int i = 0, value;
	while (ram >> std::hex >> value) {
		RAM[i] = value; //value & 0xfff;
		std::cout << i++ << std::hex << ": " << std::setfill('0') << std::setw(3)
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

void load_rom() {
	std::ifstream addr("addr.txt");

	//print addr
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

void load_uprog() {
	std::ifstream uprog("uprog.txt");

	//print control ROM
	std::cout << "Contents of control ROM with active signals identified" << std::endl
	          << "addr contents" << std::endl;
	int i = 0, value;
	while (uprog >> std::hex >> value) {
		ctrlROM[i] = value & 0xffffff;

		//Print out uprog contents
		std::cout << std::setfill(' ') << std::setw(2) << i++ << ": "
		<< std::setfill('0') << std::setw(6) << std::hex << value << " ";

		for(int code = 0; code < 19; code++)
		{
			int mask = (std::pow(2, 23 - code));
			int result = mask & value;
			if(result)
				std::cout << ops[code] << " ";
			else {
				if (code == 4 || code == 5 || code == 12 || code == 13) std::cout << "  ";
				else if (code == 17 || code == 18) std::cout << "    ";
				std::cout << "   ";
			}
		}

		int crja = value & 31;
		std::cout << " crja=" << std::setfill('0') << std::setw(2) << std::hex
							<< crja	<< std::endl;
	}
	uprog.close();
}

//Sets control signals for current word so that execute can run
void set_signals(int word) {
	int value = ctrlROM[word];

	for(int code = 0; code < 19; code++)
	{
		int mask = (std::pow(2, 23 - code));
		int result = mask & value;
		if(result)
		{
			signals[code] = true;
		}
		else
		{
			signals[code] = false;
		}
	}
	crja = value & 31;
}

//prints out signals from previous instruction
void print_cROM(int word) {
		int value = ctrlROM[word];

		//uc - microcounter
		std::cout << std::hex <<std::setfill('0') << std::setw(2) << microCntr << " ";

		for(int code = 0; code < 19; code++)
		{
			int mask = (std::pow(2, 23 - code));
			int result = mask & value;
			if(result)
				std::cout << 1;
			else
				std::cout << 0;
			if(code == 15 || code == 18)
				std::cout << " ";
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
		bus = ir;
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

		//NOT COMPLETE!!!
	if(signals[17]) // MAP
		microCntr = addrROM[mdr >> 8];// 100];
	if(!signals[17] && !signals[16])
		microCntr = crja;
	if(signals[16])
		microCntr = crja;

	if(signals[18])
		return 0;
	return 1;
}

void print_registers(){
	std::cout << "  " << pc << std::setfill(' ') << std::setw(4) << mar << std::setw(4) << mdr
				 << std::setw(4) << acc << std::setw(4) << alu << std::setw(4) << b << std::setw(4) << ir;


}

void print_signals(){
	for(int code = 0; code < 19; code++){
		if(signals[code])
		{
			std::cout << " " << ops[code];
		}
	}

	//new line for finished instrution
	if(microCntr == 0)
		 std::cout << std::endl << std::endl;
	else
		std::cout << std::endl;
}

void print_ram(){
	//print ram
	std::cout << "Contents of RAM memory" << std::endl
						<< "addr value" << std::endl;

for(int i = 0; i < 23; i++){
		std::cout << i << std::hex << ": " << std::setfill('0') << std::setw(3)
							<< RAM[i] << std::endl;
	}
}
