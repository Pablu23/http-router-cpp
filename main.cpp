#include "http.hpp"
#include "router.hpp"

using namespace http;

void HelloWorld(Request req, Response *res) {
  res->SetPayload("Hello World!");
  res->SetContentType("text/plain");
}

int main() {
  Router router(8181);

  // Allow all Methods
  router.Handle("GET /helloWorld", HelloWorld);
  router.Handle("GET /healthz", [](Request req, Response *res) {
    res->SetStatusCode(statuscode::OK);
    res->SetPayload(std::vector<std::byte>());
    res->SetContentType("text/plain");
  });

  // Only allow GET
  router.Handle("GET /echo/{name}", [](Request req, Response *res) {
    std::string name = req.path.Get("name").value_or("No Name given");
    res->SetPayload("Hello " + name);
    res->SetContentType("text/plain");
  });

  // Only allow POST
  router.Handle("POST /echo/{name}", [](Request req, Response *res) {
    std::string name = req.path.Get("name").value_or("No Name given");
    res->SetPayload("Hello with Post" + name);
    res->SetContentType("text/plain");
  });

  router.Handle("GET /", [](Request req, Response *res) {
    res->SetPayload("Main");
    res->SetContentType("text/plain");
  });

  router.Start();
  return 0;
}
