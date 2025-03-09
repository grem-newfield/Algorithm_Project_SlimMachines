#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>

extern sqlite3 *db;
int create_database();
int insert_user_data(const std::string &cookie_uuid, const std::string &data_json, const std::string &expiration_date);
std::string fetch_user_data(const std::string &cookie_uuid);
void delete_expired_cookies();

#endif
