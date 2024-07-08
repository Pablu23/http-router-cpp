run: build
	./server

build:
	g++ -std=c++20 main.cpp router.cpp path.cpp http.hpp request.cpp response.cpp -g -o server
