#include <crow.h>
#include <sstream>
#include <iostream>
#include <sqlite3.h>

std::string generate_new_user_id() {
    std::random_device rd; // Obtain a random number from hardware
    std::mt19937 eng(rd()); // Seed the generator
    std::uniform_int_distribution<> distr(0, 15); // Define the range

    std::stringstream ss;
    ss << std::hex; // Output in hexadecimal

    for (int i = 0; i < 8; i++) { // Create a 32-character hexadecimal number
        ss << distr(eng);
    }

    return ss.str();
}

bool validate_user_id(const std::string& user_id) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    bool result = false;

    if (sqlite3_open("user_database.db", &db) != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    std::string sql = "SELECT user_id FROM users WHERE user_id = ?";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, user_id.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            result = true; // User ID found
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Failed to execute query: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_close(db);
    return result;
}
bool insert_new_user(const std::string& user_id) {
    sqlite3* db;
    char* err_message = 0;
    int rc;

    rc = sqlite3_open("user_database.db", &db);
    if (rc) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    std::string sql = "INSERT INTO users (user_id) VALUES ('" + user_id + "');";

    rc = sqlite3_exec(db, sql.c_str(), 0, 0, &err_message);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << err_message << std::endl;
        sqlite3_free(err_message);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    return true;
}



inline void def_routes(crow::SimpleApp &app) {
    // Route to handle user session initialization
    CROW_ROUTE(app, "/start_session")([](const crow::request& req) {
        std::string cookie = req.get_header_value("Cookie");
        std::string user_id;

        if (!cookie.empty()) {
            // Parse the cookie to find the user ID
            size_t start_pos = cookie.find("user_id=");
            if (start_pos != std::string::npos) {
                start_pos += 8; // Length of "user_id="
                size_t end_pos = cookie.find(';', start_pos);
                if (end_pos == std::string::npos) {
                    end_pos = cookie.length();
                }
                user_id = cookie.substr(start_pos, end_pos - start_pos);
            }
        }

        if (user_id.empty()) {
            // No valid cookie found, generate a new user ID
            user_id = generate_new_user_id();  // Function to generate a new unique user ID
            insert_new_user(user_id);          // Insert the new user ID into the database

            // Create a cookie for the new user ID
            crow::response res("New user session started");
            res.add_header("Set-Cookie", "user_id=" + user_id + "; Path=/; HttpOnly");
            return res;
        } else {
            // Validate user ID or fetch additional data as needed
            if (validate_user_id(user_id)) {   
                crow::response res("User session resumed: " + user_id);
                return res;
            } else {
                crow::response res(400, "Invalid user session");
                return res;
            }
        }
    });
}
