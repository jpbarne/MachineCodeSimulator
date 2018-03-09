#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>
#include <string>
#include <cmath>

void printFile(const std::ifstream&);

int main(int argc, char const *argv[]) {
	std::ifstream ram, addr, uprog;
	ram.open("ram.txt");
	addr.open("addr.txt");
	uprog.open("uprog.txt");

	std::map <int, std::string> ops  
		{{0, "IP"}, {1, "LP"}, {2, "EP"}, {3, "LM"}, {4, "R"}, {5, "W"},
		 {6, "LD"}, {7, "ED"}, {8, "LI"}, {9, "EI"}, {10, "LA"},
		 {11, "EA"}, {12, "A"}, {13, "S"}, {14, "EU"}, {15, "LB"},
		 {16, "CD"}, {17, "MAP"}, {18, "HLT"}};

	//print ram
	std::cout << "Contents of RAM memory" << std::endl
	          << "addr value" << std::endl;
	int i = 0, value;
	while (ram >> value) {
		std::cout << i++ << ": " << std::setfill('0') << std::setw(3) 
		          << value << std::endl;
	}

	//print addr
	std::cout << "Contents of address ROM" << std::endl
	          << "opc addr" << std::endl;
	i = 0;
	while (addr >> std::hex >> value) {
		std::cout << i++ << ": " << std::setfill('0') << std::setw(2) 
		          << std::hex << value << std::endl;
	}
	
	//print control ROM
	std::cout << "Contents of control ROM with active signals identified" << std::endl
	          << "addr contents" << std::endl;
	i = 0;
	while (uprog >> std::hex >> value) {
		//Print out addr contents
		std::cout << std::setfill(' ') << std::setw(2) << i++ << ": " 
		<< std::setfill('0') << std::setw(6) << std::hex << value;

		for(int code = 0; code < 19; code++)
		{
			int mask = (std::pow(2, 23 - code));
			int result = mask & value;
			if(result)
				std::cout << " " << ops[code];
			else
				std::cout << "   ";
		}

		std::cout << std::endl;
	}

	ram.close();
	addr.close();
	uprog.close();
	return 0;
}
