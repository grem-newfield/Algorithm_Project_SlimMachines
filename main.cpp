#include "lodepng.h"
#include <chrono>
#include <crow.h>
#include <crow/http_response.h>
#include <crow/mustache.h>
#include <iostream>
#include <sqlite3.h>
#include <string>
#include <vector>

void debug(const char *s) { std::cout << "DEBUG: " << s << std::endl; }
void debug(std::string s) { std::cout << "DEBUG: " << s << std::endl; }

#include "graph.h"

int create_database(sqlite3 *&db) {
  int rc = sqlite3_open("example.db", &db);
  if (rc) {
    std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
    return rc;
  }

  const char *sql = "CREATE TABLE IF NOT EXISTS Users (ID INTEGER PRIMARY KEY "
                    "AUTOINCREMENT, Data BLOB NOT NULL);";
  char *errMsg = nullptr;
  rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error: " << errMsg << std::endl;
    sqlite3_free(errMsg);
  }
  return rc;
}

int main() {
  crow::SimpleApp app;
  sqlite3 *db = nullptr;

  if (create_database(db) != SQLITE_OK) {
    return 1;
  }

  CROW_ROUTE(app, "/generate/image")([]() {
    crow::response res;
    res.set_header("Connection", "Close");
    res.set_header("Content-Type", "image/png");
    std::vector<int> test_data = {70,  80,  90,  100, 110,
                                  120, 130, 140, 150, 160};
    std::shuffle(test_data.begin(), test_data.end(),
                 std::default_random_engine());
    int w = 1024;
    int h = 300;
    auto png = generate_png(w, h, test_data);
    std::cout << "png size:" << png.size();
    auto scaled_png = rescale(png, w, h, w * 2, h * 2, false);
    std::cout << "scaled png size:" << scaled_png.size();
    res.write(std::string(scaled_png.begin(), scaled_png.end()));
    return res;
  });
  // CROW_ROUTE(app, "/")([]() {
  //   auto page = crow::mustache::load("index.html");
  //   std::string name = "Giovanni";
  //   crow::mustache::context ctx({{"user", name}});
  //   crow::response res(page.render(ctx));
  //   res.set_header("Connection", "Close");
  //   return res;
  // });

  CROW_ROUTE(app, "/")([]() {
    auto page = crow::mustache::load("index.html");

    crow::mustache::context ctx;
    ctx["user"] = "Giovanni"; // Store key-value pairs in context directly

    crow::response res(page.render(ctx));
    res.set_header("Connection", "Close");

    return res;
  });

  CROW_ROUTE(app, "/upload")
      .methods(crow::HTTPMethod::POST)([](const crow::request &req) {
        auto body = req.body;
        crow::json::rvalue formData = crow::json::load(body);
        // debug("form data: " + std::string(formData));

        if (!formData) {
          auto res = crow::response(400, "Invalid form data");
          res.set_header("Connection", "Close");
          return res;
        }

        // Access form fields
        std::string dataInput = formData["dataInput"].s();

        // Process the form data as needed
        std::ostringstream os;
        os << "Received data: " << dataInput;

        auto res = crow::response(os.str());
        res.set_header("Connection", "Close");
        return res;
      });

  app.port(8080).multithreaded().run();

  sqlite3_close(db);
  return 0;
}
