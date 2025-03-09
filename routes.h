#pragma once
#include "database.h"
#include "graph.h"
#include <crow.h>
#include <sstream>
#include <vector>
#include <random>



inline void def_routes(crow::SimpleApp &app) {

  CROW_ROUTE(app, "/")([]() {
    auto page = crow::mustache::load("index.html");
    std::string db_path = "database.db";
    std::string cookie_uuid = get_cookie_uuid_from_db(db_path);
    crow::mustache::context ctx({{"user", cookie_uuid}});
    // ctx["user"] = "Giovanni"; //or store key-value pairs in context directly
    crow::response res(page.render(ctx));
    res.set_header("Connection", "Close");
    return res;
  });

  CROW_ROUTE(app, "/generate/image")([]() {
    crow::response res;
    res.set_header("Connection", "Close");
    res.set_header("Content-Type", "image/png");
    std::vector<int> test_data = get_data_from_db(db_path);

    if(test_data.empty()){
      return crow__response(500, "Error: No data found in database");
      
    }
                                  
    std::shuffle(test_data.begin(), test_data.end(),
                 std::default_random_engine());
    int w = 1024;
    int h = 300;
    auto png = generate_png(w, h, test_data);
    res.write(std::string(png.begin(), png.end()));
    return res;
  });

  CROW_ROUTE(app, "/upload")
      .methods(crow::HTTPMethod::POST)([](const crow::request &req) {
        auto body = req.body;
        crow::json::rvalue form_data = crow::json::load(body);

        if (!form_data) {
          auto res = crow::response(400, "invalid form data");
          res.set_header("connection", "close");
          return res;
        }

        // access form fields
        if (!form_data.has("data_input") ||
            form_data["data_input"].t() != crow::json::type::String) {
          auto res =
              crow::response(400, "data_input is missing or not a string");
          res.set_header("connection", "close");
          return res;
        }

        std::string data_input = form_data["data_input"].s();
        int number;

        try {
          number = std::stoi(data_input);
        } catch (const std::invalid_argument &) {
          auto res = crow::response(400, "data_input is not a valid integer");
          res.set_header("connection", "close");
          return res;
        } catch (const std::out_of_range &) {
          auto res =
              crow::response(400, "data_input is out of range for an integer");
          res.set_header("connection", "close");
          return res;
        }

        // process the form data as needed
        std::ostringstream os;
        os << "received data: " << number;

        auto res = crow::response(os.str());
        res.set_header("connection", "close");
        return res;
      });

  // CROW_ROUTE(app, "/upload")
  //     .methods(crow::HTTPMethod::POST)([](const crow::request &req) {
  //       auto body = req.body;
  //       // auto formdata = parseFormData(body);
  //       auto formdata = req.body;
  //
  //       if (formdata.find("data_input") == formdata.end()) {
  //         auto res = crow::response(400, "data_input is missing");
  //         res.set_header("connection", "close");
  //         return res;
  //       }
  //
  //       std::string data_input = formdata["data_input"];
  //       int number;
  //
  //       try {
  //         number = std::stoi(data_input);
  //       } catch (const std::invalid_argument &) {
  //         auto res = crow::response(400, "data_input is not a valid
  //         integer"); res.set_header("connection", "close"); return res;
  //       } catch (const std::out_of_range &) {
  //         auto res =
  //             crow::response(400, "data_input is out of range for an
  //             integer");
  //         res.set_header("connection", "close");
  //         return res;
  //       }
  //
  //       std::ostringstream os;
  //       os << "received data: " << number;
  //
  //       auto res = crow::response(os.str());
  //       res.set_header("connection", "close");
  //       return res;
  //     });

  // API route to add user data
  CROW_ROUTE(app, "/add_user_data")
      .methods(crow::HTTPMethod::POST)([](const crow::request &req) {
        auto body = crow::json::load(req.body);
        if (!body) {
          return crow::response(400, "Invalid JSON");
        }

        std::string cookie_uuid = body["cookie_uuid"].s();
        std::string data = body["data"].s();
        std::string expiration_date = body["expiration_date"].s();

        if (insert_user_data(cookie_uuid, data, expiration_date) ==
            SQLITE_DONE) {
          return crow::response(200, "Data inserted successfully");
        } else {
          return crow::response(500, "Database insertion failed");
        }
      });

  // API route to get user data
  CROW_ROUTE(app, "/get_user_data")
      .methods(crow::HTTPMethod::GET)([](const crow::request &req) {
        auto params = req.url_params;
        std::string cookie_uuid = params.get("cookie_uuid");
        if (cookie_uuid.empty()) {
          return crow::response(400, "Missing cookie_uuid parameter");
        }

        std::string data = fetch_user_data(cookie_uuid);
        if (data.empty()) {
          return crow::response(404, "Data not found");
        }

        return crow::response(200, data);
      });

  // API route to delete expired cookies
  CROW_ROUTE(app, "/delete_expired").methods(crow::HTTPMethod::POST)([]() {
    delete_expired_cookies();
    return crow::response(200, "Expired cookies deleted.");
  });
}
