.PHONY:server client dict
server:./src/config.cpp ./src/textQueryServer.cpp ./src/edit_distance.cpp
	g++ -o $@ $^ -lpthread -I ./include
client:./client.cpp ./src/config.cpp
	g++ -o $@ $^ -lpthread -I ./include
dict: ./src/config.cpp ./src/diction.cpp ./src/gbk2utf_8.cpp
	g++ -o $@ $^ -I ./include -lNLPIR -DOS_LINUX
