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

//******Functions*******//

//load and read files
void load_ram();
void load_rom();
void load_uprog();

//print while executing
void print_cROM();

//instructrion fetch routine
void fetch() {
	mar = pc;
	mdr = RAM[mar];
	ir = mdr;
	pc++;
}

//set of instruction execution routines
void inv() {
	std::cout << "invalid opcode" << std::endl;
	exit(1);
}

void lda() {
	mar = ir & 0xff;
	mdr = RAM[mar];
	acc = mdr;
}

void sta() {
	mar = ir & 0xff;
	mdr = acc;
	RAM[mar] = mdr;
}

void add() {
	alu = (acc + b) & 0xfff;
	acc = alu;
}

void sub() {
	alu = (acc - b) & 0xfff;
	acc = alu;
}

void mba() {
	b = acc;
}

void jmp() {
	pc = ir & 0xff;
}

void jneg() {
	if (acc >> 11) {
		pc = ir & 0xff;
	}
}

void hlt() {
	halt = 1;
}

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
	
	print_cROM();

	return 0;
}

void load_ram() {
	std::ifstream ram("ram.txt");

	//print ram
	std::cout << "Contents of RAM memory" << std::endl
	          << "addr value" << std::endl;
	int i = 0, value;
	while (ram >> value) {
		RAM[i] = value & 0xfff;
		std::cout << i++ << ": " << std::setfill('0') << std::setw(3)
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

void print_cROM() {
	int i = 0;
	while(ctrlROM[i] != 0xffffff)
	{
		for (int j = 0; j < 19; j++) {

		i++;
	}
}
