#include "http.hpp"
#include "router.hpp"

using namespace http;

void HelloWorld(Request req, Response *res) {
  res->set_payload("Hello World!");
  res->set_content_type("text/plain");
}

int main() {
  Router router(8181);

  // Allow all Methods
  router.handle("GET /helloWorld", HelloWorld);
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

  router.start();
  return 0;
}
