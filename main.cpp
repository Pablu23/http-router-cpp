#include "http.hpp"
#include "router.hpp"
#include <csignal>
#include <cstdlib>
#include <iostream>

using namespace http;

void hello_world(Request req, Response *res) {
  res->set_payload("Hello World!");
  res->set_content_type("text/plain");
}

static Router router(8181);

void quit(int s) {
  router.stop();
  std::cout << "Stopping Server" << std::endl;
}

int main() {
  struct sigaction sig_int_handler;
  sig_int_handler.sa_handler = quit;
  sigemptyset(&sig_int_handler.sa_mask);
  sig_int_handler.sa_flags = 0;

  sigaction(SIGINT, &sig_int_handler, nullptr);

  // Allow all Methods
  router.handle("GET /helloWorld", hello_world);
  router.handle("GET /healthz", [](Request req, Response *res) {
    res->set_status_code(StatusCode::OK);
    res->set_payload(std::vector<std::byte>());
    res->set_content_type("text/plain");
  });

  // Only allow GET
  router.handle("GET /echo/{name}", [](Request req, Response *res) {
    std::string name = req.path.get("name").value_or("No Name given");
    res->set_payload("Hello " + name);
    res->set_content_type("text/plain");
  });

  // Only allow POST
  router.handle("POST /echo/{name}", [](Request req, Response *res) {
    std::string name = req.path.get("name").value_or("No Name given");
    res->set_payload("Hello with Post" + name);
    res->set_content_type("text/plain");
  });

  router.handle("GET /", [](Request req, Response *res) {
    res->set_payload("Main");
    res->set_content_type("text/plain");
  });

  std::cout << "Starting Server" << std::endl;
  router.start();
  return 0;
}
