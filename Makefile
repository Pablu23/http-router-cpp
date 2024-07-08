run: build
	./server

build:
	g++ -std=c++20 server.cpp request.cpp response.cpp -g -o server
