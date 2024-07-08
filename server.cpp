#include "request.hpp"
#include "response.hpp"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <ostream>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

int main() {
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(8080);
  serverAddress.sin_addr.s_addr = INADDR_ANY;

  int err = bind(serverSocket, (struct sockaddr *)&serverAddress,
                 sizeof(serverAddress));
  if (err != 0) {
    std::cout << "error binding " << strerror(errno) << std::endl;
    exit(1);
  }

  err = listen(serverSocket, 5);
  if (err != 0) {
    std::cout << "error listening " << strerror(errno) << std::endl;
    exit(1);
  }

  std::vector<std::byte> buffer(1024);
  while (true) {
    int clientSocket = accept(serverSocket, nullptr, nullptr);
    int read = recv(clientSocket, buffer.data(), buffer.size(), 0);
    buffer.resize(read);
    Request req(buffer);
    Response res = Response("Hello world!");
    if (req.HasData()) {
      auto data = req.Data();
      res = Response(data);
    }

    res.SetContentType("text/plain");
    res.Send(clientSocket);
    close(clientSocket);
  }

  if (shutdown(serverSocket, SHUT_RDWR) < 0) {
    std::cout << "error shutdown " << strerror(errno) << std::endl;
    exit(1);
  }

  if (close(serverSocket) < 0) {
    std::cout << "error closing socket " << strerror(errno) << std::endl;
    exit(1);
  }

  return 0;
}
