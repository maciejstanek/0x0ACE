all: cpu

cpu: clean main.cxx
	g++ -std=c++11 main.cxx -o acpu

clean:
	rm -f acpu
	rm -f dump.html
	rm -f regs.json
	rm -f rom.bin
