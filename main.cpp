#include "request.hpp"
#include "response.hpp"
#include "router.hpp"

int main() {

  Router router(8080);
  // Allow all Methods
  router.Handle("/helloWorld", [](Request req, Response res) -> Response {
    res.SetPayload("Hello World!");
    res.SetContentType("text/plain");
    return res;
  });
  
  // Only allow GET
  router.Handle("GET /echo/{name}", [](Request req, Response res) -> Response {
    std::string name = req.path.Get("name").value_or("No Name given");
    res.SetPayload("Hello " + name);
    res.SetContentType("text/plain");
    return res;
  });

  // Only allow POST
  router.Handle("POST /echo/{name}", [](Request req, Response res) -> Response {
    std::string name = req.path.Get("name").value_or("No Name given");
    res.SetPayload("Hello with Post" + name);
    res.SetContentType("text/plain");
    return res;
  });

  router.Handle("/", [](Request req, Response res) -> Response {
    res.SetPayload("Main");
    res.SetContentType("text/plain");
    return res;
  });

  router.Start();
  return 0;
}
