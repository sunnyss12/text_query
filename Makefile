main:./src/*.cpp
	g++ -o main ./src/*.cpp -lpthread -lNLPIR -I ./include -DOS_LINUX
