.PHONY:server client dict
server:./src/config.cpp ./src/textQueryServer.cpp ./src/textQueryTask.cpp ./src/edit_distance.cpp ./src/index.cpp ./src/cache.cpp
	g++  -o ./bin/$@ $^ -lpthread -I ./include
client:./client.cpp ./src/config.cpp
	g++ -o ./bin/$@ $^ -lpthread -I ./include
dict: ./src/config.cpp ./src/diction.cpp ./src/index.cpp ./src/gbk2utf_8.cpp ./src/common.cpp
	g++ -o ./bin/$@ $^ -I ./include -lNLPIR -DOS_LINUX -DBUILD_INDEX
