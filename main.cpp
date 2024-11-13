#include "http.hpp"
#include "router.hpp"
#include "tree.hpp"

using namespace http;

void HelloWorld(Request req, Response *res) {
  res->SetPayload("Hello World!");
  res->SetContentType("text/plain");
}

int main() {
  Tree t;

  t.AddPath("/test/dummy", [](Request req, Response *res) {
    res->SetPayload("Main");
    res->SetContentType("text/plain");
  });

  t.AddPath("/test/dummy/main", [](Request req, Response *res) {
    res->SetPayload("Main");
    res->SetContentType("text/plain");
  });

  t.AddPath("/test/dummy/main2", [](Request req, Response *res) {
    res->SetPayload("Main");
    res->SetContentType("text/plain");
  });

  t.AddPath("/test/dummy2/main", [](Request req, Response *res) {
    res->SetPayload("Main");
    res->SetContentType("text/plain");
  });

  t.AddPath("/var/main", [](Request req, Response *res) {
    res->SetPayload("Main");
    res->SetContentType("text/plain");
  });

  t.AddPath("/test/dummy2", [](Request req, Response *res) {
    res->SetPayload("Main");
    res->SetContentType("text/plain");
  });

  t.DebugPrint();

  // Router router(8181);
  //
  // // Allow all Methods
  // router.Handle("/helloWorld", HelloWorld);
  //
  // router.Handle("/healthz", [](Request req, Response *res) {
  //   res->SetStatusCode(statuscode::OK);
  //   res->SetPayload(std::vector<std::byte>());
  //   res->SetContentType("text/plain");
  // });
  //
  // // Only allow GET
  // router.Handle("GET /echo/{name}", [](Request req, Response *res) {
  //   std::string name = req.path.Get("name").value_or("No Name given");
  //   res->SetPayload("Hello " + name);
  //   res->SetContentType("text/plain");
  // });
  //
  // // Only allow POST
  // router.Handle("POST /echo/{name}", [](Request req, Response *res) {
  //   std::string name = req.path.Get("name").value_or("No Name given");
  //   res->SetPayload("Hello with Post" + name);
  //   res->SetContentType("text/plain");
  // });
  //
  // router.Handle("/", [](Request req, Response *res) {
  //   res->SetPayload("Main");
  //   res->SetContentType("text/plain");
  // });
  //
  // router.Start();
  // return 0;
}
