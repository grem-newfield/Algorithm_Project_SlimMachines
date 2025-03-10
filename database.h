#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>

extern sqlite3 *db;
int create_database();
int insert_or_update_user_data(const std::string &cookie_uuid, const std::string &data_json, const std::string &expiration_date);
std::string fetch_user_data(const std::string &cookie_uuid);
std::vector <int> get_data_from_db(const std::string &db_path);
std:: string get_cookie_uuid_from_db(const std:: string &db_path)
void delete_expired_cookies();

#endif
