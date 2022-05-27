.PHONY: main

all: main

main:
	g++ -O2 sdk/main.cpp sdk/json/jsoncpp.cpp Judge.cpp Strategy.cpp Node.cpp Uct.cpp -o main -std=c++11
