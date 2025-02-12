#include "lodepng.h"
#include "database.h"
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

// Function to initialize the database

int main()
{
    crow::SimpleApp app;
   

    if (create_database() != SQLITE_OK)
    {
        return 1;
    }

    CROW_ROUTE(app, "/generate/image")([]()
    {
        crow::response res;
        res.set_header("Connection", "Close");
        res.set_header("Content-Type", "image/png");

        // Ensure `generate_png()` is implemented
        std::vector<unsigned char> png = generate_png();
        res.write(std::string(png.begin(), png.end()));

        return res;
    });

    CROW_ROUTE(app, "/")([]()
    {
        auto page = crow::mustache::load("index.html");

        crow::mustache::context ctx;
        ctx["user"] = "Giovanni";

        crow::response res(page.render(ctx));
        res.set_header("Connection", "Close");

        return res;
    });

    // API route to add user data
    CROW_ROUTE(app, "/add_user_data").methods(crow::HTTPMethod::POST)([](const crow::request &req)
    {
        auto body = crow::json::load(req.body);
        if (!body)
        {
            return crow::response(400, "Invalid JSON");
        }

        std::string cookie_uuid = body["cookie_uuid"].s();
        std::string data = body["data"].s();
        std::string expiration_date = body["expiration_date"].s();

        if (insert_user_data(cookie_uuid, data, expiration_date) == SQLITE_DONE)
        {
            return crow::response(200, "Data inserted successfully");
        }
        else
        {
            return crow::response(500, "Database insertion failed");
        }
    });

    // API route to get user data
    CROW_ROUTE(app, "/get_user_data").methods(crow::HTTPMethod::GET)([](const crow::request &req)
    {
        auto params = req.url_params;
        std::string cookie_uuid = params.get("cookie_uuid");
        if (cookie_uuid.empty())
        {
            return crow::response(400, "Missing cookie_uuid parameter");
        }

        std::string data = fetch_user_data(cookie_uuid);
        if (data.empty())
        {
            return crow::response(404, "Data not found");
        }

        return crow::response(200, data);
    });

    // API route to delete expired cookies
    CROW_ROUTE(app, "/delete_expired").methods(crow::HTTPMethod::POST)([]()
    {
        delete_expired_cookies();
        return crow::response(200, "Expired cookies deleted.");
    });

    app.port(8080).multithreaded().run();

    sqlite3_close(db); // Close the database on exit
    return 0;
}
