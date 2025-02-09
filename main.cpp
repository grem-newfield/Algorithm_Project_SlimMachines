#include "database.h"
#include "lodepng.h"
#include "routes.h"
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

int main() {
  crow::SimpleApp app;
  sqlite3 *db = nullptr;

  if (create_database() != SQLITE_OK) {
    std::cout << "FATAL: FAILED TO CREATE DATABASE\n";
    return 1;
  }

  def_routes(app);

  app.port(8080).multithreaded().run();

  sqlite3_close(db);
  return 0;
}
