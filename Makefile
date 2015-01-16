.PHONY:main client
main:./src/*.cpp
	g++ -o main ./src/*.cpp -lpthread -lNLPIR -I ./include -DOS_LINUX
client:	./client.cpp ./src/config.cpp
	g++ -o client client.cpp ./src/config.cpp -I ./include -lpthread
