#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <bitset>

using namespace std;
/*
typedef enum ace_code {
	ace_code_move = 0x00,
	ace_code_bitwise_or = 0x01,
	ace_code_bitwise_xor = 0x02,
	ace_code_bitwise,
};
*/
int main(int argc, char *argv[]) {

	if(argc != 2) {
		cout << "Error, please provide a binary file as an argument. Aborting" << endl;
		return 122;
	}

	printf("\n\e[1;41m ======= 0x0ACE ======= \e[0m\n");

	ifstream binaryFile(argv[1], ios::in | ios::binary | ios::ate);
	if(!binaryFile.is_open()) {
		printf("Error while opening the binary file '%s'. Aborting.\n", argv[1]);
		return 123;
	}
	ifstream::pos_type binarySize = binaryFile.tellg();
	char *temp = new char[binarySize];
	binaryFile.seekg(0, ios::beg);
	if(!binaryFile.read(temp, binarySize)) {
		printf("Error while reading the binary file '%s'. Aborting.\n", argv[1]);
		return 124;
	}

	cout << binarySize << " / 4 = " << (float)binarySize/(float)(sizeof(char)*4) << endl;
	for(int i = 0; i< binarySize; i++) {
		//printf("%02x", (unsigned char)temp[i]);
		int code = 40;
		if((unsigned char)temp[i] <= 16) {
			code += 1;
		}
		cout << "\e[" << code << "m" << bitset<8>(temp[i]) << "\e[0m ";
		printf("%02x ", (unsigned char)temp[i]);

		printf("\e[%dmdest->r%d\e[0m \e[%dmsrc<-r%d\e[0m ", ((temp[i]>>2)&3)+31, (temp[i]>>2)&3, (temp[i]&3)+31, temp[i]&3);
		switch(temp[i]) {
			case 0x00: printf("\e[32m0x00 move                            \e[0m"); break;
			case 0x01: printf("\e[33m0x01 bitwise or                      \e[0m"); break;
			case 0x02: printf("\e[33m0x02 bitwise xor                     \e[0m"); break;
			case 0x03: printf("\e[33m0x03 bitwise and                     \e[0m"); break;
			case 0x04: printf("\e[33m0x04 bitwise negation                \e[0m"); break;
			case 0x05: printf("\e[34m0x05 addition                        \e[0m"); break;
			case 0x06: printf("\e[34m0x06 subtraction                     \e[0m"); break;
			case 0x07: printf("\e[34m0x07 multiplication                  \e[0m"); break;
			case 0x08: printf("\e[35m0x08 shift left                      \e[0m"); break;
			case 0x09: printf("\e[35m0x09 shift right                     \e[0m"); break;
			case 0x0a: printf("\e[34m0x0a increment                       \e[0m"); break;
			case 0x0b: printf("\e[34m0x0b decrement                       \e[0m"); break;
			case 0x0c: printf("\e[36m0x0c push on stack                   \e[0m"); break;
			case 0x0d: printf("\e[36m0x0d pop from stack                  \e[0m"); break;
			case 0x0e: printf("\e[37m0x0e compare                         \e[0m"); break;
			case 0x0f: printf("\e[37m0x0f jump to nth opcode when not zero\e[0m"); break;
			case 0x10: printf("\e[37m0x10 jump to nth opcode when zero    \e[0m"); break;
			default  : printf(" \e[41minvalid cmd\e[0m                         "); break;
		}
		printf(" ");
		switch((temp[i]>>4)&0xf) {
			case 0x00: printf("imm    "); break;
			case 0x01: printf("reg    "); break;
			case 0x02: printf("reg+imm"); break;
			case 0x03: printf("reg+reg"); break;
			default  : printf("\e[41minvalid\e[0m"); break;
		}
		printf("\n");

		/*
		if(i%8 == 3) {
			printf("  ");
		}
		if(i%8 == 7) {
			printf("\n");
		}
		*/
	}
	printf("\n");

	delete [] temp;
	binaryFile.close();

	return 0;
}
