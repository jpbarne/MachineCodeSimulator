#include <iostream>
#include <fstream>
#include <iomanip>

void printFile(const std::ifstream&);

int main(int argc, char const *argv[]) {
	std::ifstream ram, addr, uprog;
	ram.open("ram.txt");
	addr.open("addr.txt");
	uprog.open("uprog.txt");

	//print ram
	std::cout << "Contents of RAM memory" << std::endl
	          << "addr value" << std::endl;
	int i = 0, value;
	while (ram >> value) {
		std::cout << i << ": " << std::setfill('0') << std::setw(3) << value << std::endl;
		i++;
	}

	//print addr
	std::cout << "Contents of address ROM" << std::endl
	          << "opc value" << std::endl;
	i = 0;
	while (addr >> std::hex >> value) {
		std::cout << i << ": " << std::setfill('0') << std::setw(2) << std::hex << value << std::endl;
		i++;
	}

	ram.close();
	addr.close();
	uprog.close();
	return 0;
}
