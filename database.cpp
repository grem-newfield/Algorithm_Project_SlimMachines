#include "database.h"
#include <iostream>

sqlite3 *db;

int create_database()
{
    int rc = sqlite3_open("health_data.db", &db);
    if (rc)
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    const char *sql = R"(
        CREATE TABLE IF NOT EXISTS USER_DATA (
            COOKIE_UUID TEXT PRIMARY KEY NOT NULL,
            DATA TEXT NOT NULL,
            EXPIRATION_DATE DATE NOT NULL
        );
    )";

    char *errMsg = nullptr;
    rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return rc;
    }

    return SQLITE_OK;
}

// Function to insert user data into the database
int insert_or_update_user_data(const std::string &cookie_uuid, const std::string &data_json, const std::string &expiration_date)
{
    sqlite3_stmt *stmt;
    const char *sql_check = "SELECT COUNT(*) FROM USER_DATA WHERE COOKIE_UUID = ?;";
    const char *sql_insert = "INSERT INTO USER_DATA (COOKIE_UUID, DATA, EXPIRATION_DATE) VALUES (?, ?, ?);";
    const char *sql_update = "UPDATE USER_DATA SET DATA = ?, EXPIRATION_DATE = ? WHERE COOKIE_UUID = ?;";

    // Check if the entry exists
    int rc = sqlite3_prepare_v2(db, sql_check, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL prepare error: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    sqlite3_bind_text(stmt, 1, cookie_uuid.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    bool exists = false;
    if (rc == SQLITE_ROW)
    {
        exists = sqlite3_column_int(stmt, 0) > 0;
    }
    sqlite3_finalize(stmt);

    // Insert or update based on existence
    if (exists)
    {
        rc = sqlite3_prepare_v2(db, sql_update, -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            std::cerr << "SQL prepare error: " << sqlite3_errmsg(db) << std::endl;
            return rc;
        }

        sqlite3_bind_text(stmt, 1, data_json.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, expiration_date.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, cookie_uuid.c_str(), -1, SQLITE_STATIC);
    }
    else
    {
        rc = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            std::cerr << "SQL prepare error: " << sqlite3_errmsg(db) << std::endl;
            return rc;
        }

        sqlite3_bind_text(stmt, 1, cookie_uuid.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, data_json.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, expiration_date.c_str(), -1, SQLITE_STATIC);
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        std::cerr << "SQL execution error: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    return rc;
}

// Function to fetch user data
std::string fetch_user_data(const std::string &cookie_uuid)
{
    sqlite3_stmt *stmt;
    const char *sql = "SELECT DATA FROM USER_DATA WHERE COOKIE_UUID = ?;";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL prepare error: " << sqlite3_errmsg(db) << std::endl;
        return "";
    }

    sqlite3_bind_text(stmt, 1, cookie_uuid.c_str(), -1, SQLITE_STATIC);

    std::string data;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        data = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
    }
    else
    {
        std::cerr << "No data found for cookie: " << cookie_uuid << std::endl;
    }

    sqlite3_finalize(stmt);
    return data;
}

std::vector <int> get_data_from_db(const std::string &db_path){
    sqlite3 *db;
    sqlite3_stmt *stmt;
    std::vector <int> data;
  
    if (sqlite3_open(db_path.c_str(), &db) == SQLITE_OK){
      std::string query = "SELECT value FROM health_data;";
      if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK ){
        while (sqlite3_step(stmt) == SQLITE_ROW){
          data.push_back(sqlite3_column_int(stmt, 0));
        }
      }
      sqlite3_finalize(stmt);
    }
    sqlite3_close(db);
    return data;
  }

  std:: string get_cookie_uuid_from_db(const std:: string &db_path){
    sqlite3 *db;
    sqlite3_stmt *stmt;
    std:: string cookie_uuid;

    if(sqlite3_open (db_path.c_str(), &db) == SQLITE_OK){
        std::string query = "SELECT COOKIE_UUID FROM USER DATA LIMIT 1;";
        if(sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK){
            if(sqlite3_step(stmt) == SQLITE_ROW){
                cookie_uuid = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
            }
        }
        sqlite3_finalize(stmt);
    }
    sqlite3_close(db);
    return cookie_uuid;
  }

// Function to delete expired cookies
void delete_expired_cookies()
{
    const char *sql = "DELETE FROM USER_DATA WHERE EXPIRATION_DATE < DATE('now');";

    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL delete error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    else
    {
        std::cout << "Expired cookies deleted successfully." << std::endl;
    }
}
