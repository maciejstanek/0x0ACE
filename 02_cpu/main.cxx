#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <bitset>
#include <vector>

using namespace std;

// This class describe one opcode, independent of its byte length
class Opcode
{
	protected:
		// The opcode consist of the following parts
		uint8_t code; // operation, code 0x00 to 0x10, mandatory
		uint8_t modByte; // modifiers, mandatory
		uint16_t imm; // immediate value, optional

		// The modByte consist of the follwoing parts in this order
		uint8_t srcReg; // source register (0x0 to 0x3)
		uint8_t destReg; // desctination register (0x0 to 0x3)
		uint8_t mod; // operation modifier (0x0 to 0x3)
		static int globalIndex;
		int index;

	public:
		int Validate()
		{
			if(code > 0x10) {
				printf("Operartion code 0x%02x is invalid\n", code);
				return 1;
			}
			if(srcReg > 0x3) {
				printf("Source register with index %d does not exist\n", srcReg);
				return 1;
			}
			if(destReg > 0x3) {
				printf("Destination register with index %d does not exist\n", destReg);
				return 1;
			}
			if(mod > 0x3) {
				printf("Modifier 0x%02x is invalid\n", mod);
				return 1;
			}
			return 0;
		}

		int GetIndex() {
			return index;
		}

		Opcode(uint8_t _code, uint8_t _modByte)
			: code(_code)
			, modByte(_modByte)
			, imm(0)
		{
			mod = _modByte & 0x0f;
			destReg = (_modByte & 0x30) >> 4;
			srcReg = (_modByte & 0xC0) >> 6;
			index = globalIndex++;
		}

		bool IsImmRequired()
		{
			return !(modByte&1);
		}

		void SetImm(uint16_t _imm) {
			imm = _imm;
		}

		static void PrintHeader()
		{
			cout << "\e[1m#  \u2502 opcode           imm              \u2502 operation          \u2502 modifier \u2502 src \u2502 dst \u2502 imm   \u2502\e[0m" << endl;
			cout << "\u2501\u2501\u2501\u253f\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u253f\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u253f\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u253f\u2501\u2501\u2501\u2501\u2501\u253f\u2501\u2501\u2501\u2501\u2501\u253f\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2525" << endl;
		}

		void PrintDesc()
		{
			printf("\e[32m%02d\e[0m \u2502 ", index);
			cout << bitset<8>(code) << bitset<8>(modByte) << " ";
			if(IsImmRequired()) {
				cout << bitset<16>(imm) << " \u2502 ";
			} else {
				cout << "\e[1;30m................\e[0m \u2502 ";
			}
			switch(code) {
				case 0x00: printf("\e[32mmove              \e[0m \u2502 "); break;
				case 0x01: printf("\e[33mbitwise or        \e[0m \u2502 "); break;
				case 0x02: printf("\e[33mbitwise xor       \e[0m \u2502 "); break;
				case 0x03: printf("\e[33mbitwise and       \e[0m \u2502 "); break;
				case 0x04: printf("\e[33mbitwise negation  \e[0m \u2502 "); break;
				case 0x05: printf("\e[34maddition          \e[0m \u2502 "); break;
				case 0x06: printf("\e[34msubtraction       \e[0m \u2502 "); break;
				case 0x07: printf("\e[34mmultiplication    \e[0m \u2502 "); break;
				case 0x08: printf("\e[35mshift left        \e[0m \u2502 "); break;
				case 0x09: printf("\e[35mshift right       \e[0m \u2502 "); break;
				case 0x0a: printf("\e[34mincrement         \e[0m \u2502 "); break;
				case 0x0b: printf("\e[34mdecrement         \e[0m \u2502 "); break;
				case 0x0c: printf("\e[36mpush on stack     \e[0m \u2502 "); break;
				case 0x0d: printf("\e[36mpop from stack    \e[0m \u2502 "); break;
				case 0x0e: printf("\e[37mcompare           \e[0m \u2502 "); break;
				case 0x0f: printf("\e[37mjump when not zero\e[0m \u2502 "); break;
				case 0x10: printf("\e[37mjump when zero    \e[0m \u2502 "); break;
				default  : printf("\e[31minvalid cmd       \e[0m \u2502 "); break;
			}
			switch(mod) {
				case 0x00: printf(" \e[32mimm    \e[0m \u2502 "); break;
				case 0x01: printf(" \e[33mreg    \e[0m \u2502 "); break;
				case 0x02: printf(" \e[34mreg+imm\e[0m \u2502 "); break;
				case 0x03: printf(" \e[35mreg+reg\e[0m \u2502 "); break;
				default  : printf(" \e[31minvalid\e[0m \u2502 "); break;
			}
			printf(" \e[%dmr%d\e[0m \u2502 ", srcReg+31, srcReg);
			printf(" \e[%dmr%d\e[0m \u2502", destReg+31, destReg);
			if(IsImmRequired()) {
				printf("% 6d \u2502\n", imm);
			} else {
				printf("       \u2502\n", imm);
			}
		}
};

int Opcode::globalIndex = 0;

int main(int argc, char *argv[])
{
	cout << "\n\e[1;41m ======= 0x0ACE ======= \e[0m\n\n";
	if(argc != 2) {
		cout << "Error, please provide a binary file as an argument. Aborting" << endl;
		return 122;
	}
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
	binaryFile.close();

	// Parse binary to opcodes
	vector<Opcode*> program;
	int byteIndex = 0;
	while(byteIndex < binarySize) {
		// Load a mandatory part of the opcode
		if(byteIndex + 1 >= binarySize) {
			cout << "Unexpected end of file after reading a command #" << program.back()->GetIndex() << endl;
			return 1;
		}
		Opcode *opcode = new Opcode(temp[byteIndex], temp[byteIndex+1]);
		byteIndex += 2;
		// Load an immediate value if required
		if(opcode->IsImmRequired()) {
			if(byteIndex + 1 >= binarySize) {
				cout << "Unexpected end of file while reading an immediate value of command #" << opcode->GetIndex() << endl;
				return 1;
			}
			opcode->SetImm((temp[byteIndex+1] << 8) + temp[byteIndex]);
		byteIndex += 2;
		}
		// Check sanity
		if(opcode->Validate()) {
			cout << "Command #" << opcode->GetIndex() << " is invalid" << endl;
			return 1;
		}
		// Save the opcode in a vector
		program.push_back(opcode);
	}

	// Print the parsed opcodes
	Opcode::PrintHeader();
	for(auto &opcode : program) {
		opcode->PrintDesc();
	}
/*
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
	}
	printf("\n");
*/

	delete [] temp;

	return 0;
}
