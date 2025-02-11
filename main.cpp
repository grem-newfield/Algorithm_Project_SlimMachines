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

std::vector<unsigned char> generate_png()
{
  std::chrono::time_point start = std::chrono::high_resolution_clock::now();

  unsigned width = 256, height = 256;
  std::vector<unsigned char> image(width * height * 4);
  for (unsigned y = 0; y < height; y++)
  {
    for (unsigned x = 0; x < width; x++)
    {
      image[4 * width * y + 4 * x + 0] = x % 256;
      image[4 * width * y + 4 * x + 1] = y % 256;
      image[4 * width * y + 4 * x + 2] = 128;
      image[4 * width * y + 4 * x + 3] = 255;
    }
  }
  std::vector<unsigned char> png;
  unsigned error = lodepng::encode(png, image, width, height);
  if (error)
  {
    std::cerr << "encoder error " << error << ": " << lodepng_error_text(error)
              << std::endl;
  }
  std::chrono::time_point end = std::chrono::high_resolution_clock::now();
  long nanos =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  float sec = (float)nanos / 1e9;

  debug("Generated PNG in " + std::to_string(sec) + " seconds");
  return png;
}

int create_database(sqlite3 *&db)
{
  int rc = sqlite3_open("example.db", &db);
  if (rc)
  {
    std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
    return rc;
  }

  const char *sql = "CREATE TABLE IF NOT EXISTS Users (ID INTEGER PRIMARY KEY "
                    "AUTOINCREMENT, Data BLOB NOT NULL);";
  char *errMsg = nullptr;
  rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
  if (rc != SQLITE_OK)
  {
    std::cerr << "SQL error: " << errMsg << std::endl;
    sqlite3_free(errMsg);
  }
  return rc;
}

int main()
{
  crow::SimpleApp app;
  sqlite3 *db = nullptr;

  if (create_database(db) != SQLITE_OK)
  {
    return 1;
  }

  CROW_ROUTE(app, "/generate/image")([]()
                                     {
    crow::response res;
    res.set_header("Connection", "Close");
    res.set_header("Content-Type", "image/png");
    auto png = generate_png();
    res.write(std::string(png.begin(), png.end()));
    return res; });
  // CROW_ROUTE(app, "/")([]() {
  //   auto page = crow::mustache::load("index.html");
  //   std::string name = "Giovanni";
  //   crow::mustache::context ctx({{"user", name}});
  //   crow::response res(page.render(ctx));
  //   res.set_header("Connection", "Close");
  //   return res;
  // });

  CROW_ROUTE(app, "/")([]()
                       {
    auto page = crow::mustache::load("index.html");

    crow::mustache::context ctx;
    ctx["user"] = "Giovanni"; // Store key-value pairs in context directly

    crow::response res(page.render(ctx));
    res.set_header("Connection", "Close");

    return res; });

  CROW_ROUTE(app, "/upload")
      .methods(crow::HTTPMethod::POST)([](const crow::request &req)
                                       {
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
        return res; });

  app.port(8080).multithreaded().run();

  sqlite3_close(db);
  return 0;
}
