all: cpu
	./req.sh

cpu: clean main.cxx
	g++ -std=c++11 main.cxx -o acpu

clean:
	rm -f acpu
