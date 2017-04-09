#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <bitset>
#include <vector>

using namespace std;

// class Opcode {{{
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
		// method Validate {{{
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
		// }}}
		// method GetIndex {{{
		int GetIndex() {
			return index;
		}
		// }}}
		// constructor {{{
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
		// }}}
		// method IsImmRequired {{{
		bool IsImmRequired()
		{
			return !(modByte&1);
		}
		// }}}
		// method SetImm {{{
		void SetImm(uint16_t _imm) {
			imm = _imm;
		}
		// }}}
		// static method PrintHeader {{{
		static void PrintHeader()
		{
			cout << "\e[1m#  \u2502 opcode           imm              \u2502 operation          \u2502 modifier \u2502 src \u2502 dst \u2502 imm   \u2502\e[0m" << endl;
			cout << "\u2501\u2501\u2501\u253f\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u253f\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u253f\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u253f\u2501\u2501\u2501\u2501\u2501\u253f\u2501\u2501\u2501\u2501\u2501\u253f\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2525" << endl;
		}
		// }}}
		// method PrintDesc {{{
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
				printf("       \u2502\n");
			}
		}
		// }}}
		// method GetCode() {{{
		uint8_t GetCode() {
			return code;
		}
		// }}}
		// method GetImm() {{{
		uint16_t GetImm() {
			return imm;
		}
		// }}}
		// method GetDestReg() {{{
		uint8_t GetDestReg() {
			return destReg;
		}
		// }}}
		// method GetDestReg() {{{
		uint8_t GetSrcReg() {
			return srcReg;
		}
		// }}}
		// method GetMod() {{{
		uint8_t GetMod() {
			return mod;
		}
		// }}}
};
// }}}
// class Cpu {{{
class Cpu {
	protected:
		vector<Opcode*>::iterator index;
		uint16_t r[4];
		vector<Opcode*> *program;
		int watchdog;
		bool zeroFlag;
	
	public:
		// constructor {{{
		Cpu(vector<Opcode*>* _program)
			: program(_program)
			, index(_program->begin())
			, watchdog(999)
			, zeroFlag(false)
		{
			r[0] = 0;
			r[1] = 0;
			r[2] = 0;
			r[3] = 0;
		}
		// }}}
		// method PrintStateJSON {{{
		int PrintStateJSON(const char *file) {
			FILE *fp = fopen(file, "w");
			if(!fp) {
				printf("Error while opening output file %s\n", file);
				return 1;
			}
			fprintf(fp, "{\"reg0\":\"%02x%02x\",", (r[0]&0xff), ((r[0]&0xff00)>>8));
			fprintf(fp, "\"reg1\":\"%02x%02x\",", (r[1]&0xff), ((r[1]&0xff00)>>8));
			fprintf(fp, "\"reg2\":\"%02x%02x\",", (r[2]&0xff), ((r[2]&0xff00)>>8));
			fprintf(fp, "\"reg3\":\"%02x%02x\"}", (r[3]&0xff), ((r[3]&0xff00)>>8));
			fclose(fp);
			return 0;
		}
		// }}}
	protected:
		// method Tick {{{
		// NOTE: I won't implement everything: turns out
		//       each downloaded program is similar, uses
		//       only one jump, one decrement, no stack
		//       operations, not used modifier, etc.
		int Tick() {
			--watchdog;
			Opcode *opcode = *index;
			switch(opcode->GetCode()) {
				// 0x00 move {{{
				case 0x00: {
					switch(opcode->GetMod()) {
						case 0x2: { // reg+imm
							r[opcode->GetDestReg()] = opcode->GetImm();
							index++;
							return 0;
						}
						case 0x3: { // reg+reg
							r[opcode->GetDestReg()] = r[opcode->GetSrcReg()];
							index++;
							return 0;
						}
					}
					printf("Invalid modifier 0x%x\n", opcode->GetMod());
					return 1;
				}
				// }}}
				// 0x01 bitwise or {{{
				case 0x01: {
					switch(opcode->GetMod()) {
						case 0x2: { // reg+imm
							//r[opcode->GetDestReg()] = opcode->GetImm() | r[opcode->GetSrcReg()];
							r[opcode->GetDestReg()] |= opcode->GetImm();
							zeroFlag = r[opcode->GetDestReg()]?false:true;
							index++;
							return 0;
						}
						case 0x3: { // reg+reg
							r[opcode->GetDestReg()] |= r[opcode->GetSrcReg()];
							zeroFlag = r[opcode->GetDestReg()]?false:true;
							index++;
							return 0;
						}
					}
					printf("Invalid modifier 0x%x\n", opcode->GetMod());
					return 1;
				}
				// }}}
				// 0x02 bitwise xor {{{
				case 0x02: {
					switch(opcode->GetMod()) {
						case 0x2: { // reg+imm
							//r[opcode->GetDestReg()] = opcode->GetImm() ^ r[opcode->GetSrcReg()];
							r[opcode->GetDestReg()] ^= opcode->GetImm();
							zeroFlag = r[opcode->GetDestReg()]?false:true;
							index++;
							return 0;
						}
						case 0x3: { // reg+reg
							r[opcode->GetDestReg()] ^= r[opcode->GetSrcReg()];
							zeroFlag = r[opcode->GetDestReg()]?false:true;
							index++;
							return 0;
						}
					}
					printf("Invalid modifier 0x%x\n", opcode->GetMod());
					return 1;
				}
				// }}}
				// 0x03 bitwise and {{{
				case 0x03: {
					switch(opcode->GetMod()) {
						case 0x2: { // reg+imm
							//r[opcode->GetDestReg()] = opcode->GetImm() & r[opcode->GetSrcReg()];
							r[opcode->GetDestReg()] &= opcode->GetImm();
							zeroFlag = r[opcode->GetDestReg()]?false:true;
							index++;
							return 0;
						}
						case 0x3: { // reg+reg
							r[opcode->GetDestReg()] &= r[opcode->GetSrcReg()];
							zeroFlag = r[opcode->GetDestReg()]?false:true;
							index++;
							return 0;
						}
					}
					printf("Invalid modifier 0x%x\n", opcode->GetMod());
					return 1;
				}
				// }}}
				// 0x04 bitwise negation {{{
				case 0x04: {
					switch(opcode->GetMod()) {
						case 0x1: { // reg
							//r[opcode->GetDestReg()] = ~r[opcode->GetSrcReg()];
							r[opcode->GetDestReg()] = ~r[opcode->GetDestReg()];
							zeroFlag = r[opcode->GetDestReg()]?false:true;
							index++;
							return 0;
						}
					}
					printf("Invalid modifier 0x%x\n", opcode->GetMod());
					return 1;
				}
				// }}}
				// 0x05 addition {{{
				case 0x05: {
					switch(opcode->GetMod()) {
						case 0x2: { // reg+imm
							//r[opcode->GetDestReg()] = opcode->GetImm() + r[opcode->GetSrcReg()];
							r[opcode->GetDestReg()] += opcode->GetImm();
							zeroFlag = r[opcode->GetDestReg()]?false:true;
							index++;
							return 0;
						}
						case 0x3: { // reg+reg
							r[opcode->GetDestReg()] += r[opcode->GetSrcReg()];
							zeroFlag = r[opcode->GetDestReg()]?false:true;
							index++;
							return 0;
						}
					}
					printf("Invalid modifier 0x%x\n", opcode->GetMod());
					return 1;
				}
				// }}}
				// 0x06 subtraction {{{
				case 0x06: {
					switch(opcode->GetMod()) {
						case 0x2: { // reg+imm
							//r[opcode->GetDestReg()] = opcode->GetImm() - r[opcode->GetSrcReg()];
							r[opcode->GetDestReg()] -= opcode->GetImm();
							zeroFlag = r[opcode->GetDestReg()]?false:true;
							index++;
							return 0;
						}
						case 0x3: { // reg+reg
							r[opcode->GetDestReg()] -= r[opcode->GetSrcReg()];
							zeroFlag = r[opcode->GetDestReg()]?false:true;
							index++;
							return 0;
						}
					}
					printf("Invalid modifier 0x%x\n", opcode->GetMod());
					return 1;
				}
				// }}}
				// 0x07 multiplication {{{
				case 0x07: {
					switch(opcode->GetMod()) {
						case 0x2: { // reg+imm
							r[opcode->GetDestReg()] *= opcode->GetImm();
							zeroFlag = r[opcode->GetDestReg()]?false:true;
							index++;
							return 0;
						}
						case 0x3: { // reg+reg
							r[opcode->GetDestReg()] *= r[opcode->GetSrcReg()];
							zeroFlag = r[opcode->GetDestReg()]?false:true;
							index++;
							return 0;
						}
					}
					printf("Invalid modifier 0x%x\n", opcode->GetMod());
					return 1;
				}
				// }}}
				// 0x08 shift left {{{
				case 0x08: {
					// TODO: Does shift works "mathematically" (endianess not considered)
					//       or it works "bitwise"?
					switch(opcode->GetMod()) {
						case 0x2: { // reg+imm
							r[opcode->GetDestReg()] <<= opcode->GetImm();
							zeroFlag = r[opcode->GetDestReg()]?false:true;
							index++;
							return 0;
						}
					}
					printf("Invalid modifier 0x%x\n", opcode->GetMod());
					return 1;
				}
				// }}}
				// 0x09 shift right {{{
				case 0x09: {
					switch(opcode->GetMod()) {
						case 0x2: { // reg+imm
							r[opcode->GetDestReg()] >>= opcode->GetImm();
							zeroFlag = r[opcode->GetDestReg()]?false:true;
							index++;
							return 0;
						}
					}
					printf("Invalid modifier 0x%x\n", opcode->GetMod());
					return 1;
				}
				// }}}
				// 0x0b decrement {{{
				case 0x0b: {
					switch(opcode->GetMod()) {
						case 0x1: { // reg
							//r[opcode->GetDestReg()] = r[opcode->GetSrcReg()] - 1;
							r[opcode->GetDestReg()]--;
							zeroFlag = r[opcode->GetDestReg()]?false:true;
							index++;
							return 0;
						}
					}
					printf("Invalid modifier 0x%x\n", opcode->GetMod());
					return 1;
				}
				// }}}
				// 0x0f jump to nth opcode when not zeroFlag {{{
				case 0x0f: {
					switch(opcode->GetMod()) {
						case 0x0: { // imm
							if(!zeroFlag) {
								index = program->begin() + opcode->GetImm();
								return 0;
							}
							index++;
							return 0;
						}
					}
					printf("Invalid modifier 0x%x\n", opcode->GetMod());
					return 1;
				}
				// }}}
			}
			printf("Unknown command 0x%02x\n", opcode->GetCode());
			return 1;
		}
		// }}}
	public:
		// method Run {{{
		int Run() {
			while(index != program->end()) {
				if(!watchdog) {
					cout << "Watchdog has stopped the CPU at command #" << GetIndex() << endl;
					return 1;
				}
				PrintState();
				if(Tick()) {
					cout << "The emulator failed on command #" << GetIndex() << endl;
					cout << "CPU register dump" << endl
						<< "  | r0 = " << r[0] << endl
						<< "  | r1 = " << r[1] << endl
						<< "  | r2 = " << r[2] << endl
						<< "  | r3 = " << r[3] << endl;
					return 1;
				}
			}
			PrintState();
			return 0;
		}
		// }}}
	protected:
		// method GetIndex {{{
		int GetIndex() {
			return distance(program->begin(), index);
		}
		// }}}
		// method PrintState {{{
		void PrintState() {
			printf("\e[1;30mWD=%04d\e[0m ", watchdog);
			printf("I=%02d ", GetIndex());
			printf("\e[%cmZ=%d\e[0m ", zeroFlag?'7':'0', zeroFlag);
			for(int i=0; i<4; i++) {
				printf("\e[%d;%dmR%d=%04x\e[0m ", r[i]?0:1, r[i]?31+i:30, i, r[i]);
			}
			if(index != program->end()) {
				if((*index)->GetMod() == 0x3) {
					printf("\e[%dmSRC=R%d\e[0m ", (*index)->GetSrcReg()+31, (*index)->GetSrcReg());
				} else {
					printf("\e[1;30mSRC=R%d\e[0m ", (*index)->GetSrcReg());
				}
				printf("\e[%dmDEST=R%d\e[0m ", (*index)->GetDestReg()+31, (*index)->GetDestReg());
				switch((*index)->GetCode()) {
					case 0x00: printf("CMD=MOV "); break;
					case 0x01: printf("CMD=OR  "); break;
					case 0x02: printf("CMD=XOR "); break;
					case 0x03: printf("CMD=AND "); break;
					case 0x04: printf("CMD=NEG "); break;
					case 0x05: printf("CMD=ADD "); break;
					case 0x06: printf("CMD=SUB "); break;
					case 0x07: printf("CMD=MUL "); break;
					case 0x08: printf("CMD=SHL "); break;
					case 0x09: printf("CMD=SHR "); break;
					case 0x0a: printf("CMD=INC "); break;
					case 0x0b: printf("CMD=DEC "); break;
					case 0x0c: printf("CMD=PUS "); break;
					case 0x0d: printf("CMD=POP "); break;
					case 0x0e: printf("CMD=CMP "); break;
					case 0x0f: printf("CMD=JNZ "); break;
					case 0x10: printf("CMD=JEZ "); break;
					default  : printf("CMD=??? ");
				}
				switch((*index)->GetMod()) {
					case 0x0: printf("\e[32mMOD=I   \e[0m"); break;
					case 0x1: printf("\e[33mMOD=R   \e[0m"); break;
					case 0x2: printf("\e[34mMOD=I+R \e[0m"); break;
					case 0x3: printf("\e[35mMOD=R+R \e[0m"); break;
				}
				printf("\e[%d;%dmIMM=%04x\e[0m ", (*index)->GetMod()&1?1:0,
					(*index)->GetMod()&1?30:0, (*index)->GetImm());
			}
			printf("\n");
		}
		// }}}
};
// }}}

int Opcode::globalIndex = 0;

// function main {{{
int main(int argc, char *argv[])
{
	cout << "\n\e[1m0x0ACE CPU EMULATOR STARTED\e[0m\n\n";
	// Initialization {{{
	// Validate the args
	if(argc != 3) {
		printf("\e[1;41mERROR\e[0m\nusage: %s <input.bin> <output.json>\n", argv[0]);
		return 1;
	}
	// Open the file
	ifstream binaryFile(argv[1], ios::in | ios::binary | ios::ate);
	if(!binaryFile.is_open()) {
		printf("Error while opening the binary file '%s'. Aborting.\n", argv[1]);
		return 1;
	}
	// Load the file into memory
	ifstream::pos_type binarySize = binaryFile.tellg();
	char *temp = new char[binarySize];
	binaryFile.seekg(0, ios::beg);
	if(!binaryFile.read(temp, binarySize)) {
		printf("Error while reading the binary file '%s'. Aborting.\n", argv[1]);
		return 1;
	}
	binaryFile.close();
	// }}}
	// Parse binary to opcodes {{{
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
	// }}}
	// Print the parsed opcodes {{{
	Opcode::PrintHeader();
	for(auto &opcode : program) {
		opcode->PrintDesc();
	}
	cout << endl;
	// }}}
	// Process commands {{{
	Cpu *cpu  = new Cpu(&program);
	if(cpu->Run()) {
		return 1;
	}
	cpu->PrintStateJSON(argv[2]);
	// }}}
	// Cleanup memory {{{
	delete [] temp;
	delete cpu;
	for(auto &opcode : program) {
		delete opcode;
	}
	// }}}
	cout << "\n\e[1m0x0ACE CPU EMULATOR FINISHED\e[0m\n\n";
	return 0;
}
// }}}
