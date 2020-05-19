//
// Created by Jonas Teuwen on 06/04/2020.
//
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <stdexcept>
#include "database.h"
#include <sqlite3.h>


/*
  Simple wrapper for SQLite3

  This code is Public Domain

  **CAUTION**
  1. No Warrantry
  2. This code is *NOT* safe against SQL Injection.
  3. This code can *NOT* handle BLOB or TEXT including '\0'.
  4. You should *NOT* use this for production.
 */


SQLite3::SQLite3(const std::string &database_name) : db(nullptr) {
    int err = sqlite3_open(database_name.c_str(), &db);
    if (err != SQLITE_OK) {
        std::string errstr = sqlite3_errmsg(db);
        sqlite3_close(db);
        db = nullptr;
        throw std::runtime_error(errstr);
    }
}

SQLite3::~SQLite3() {
    sqlite3_close(db);
    db = nullptr;
}

std::vector<SQLite3::Row> SQLite3::exec(const std::string &sql) {
    char *errmsg = nullptr;
    auto callback = [](void *result_, int argc,
                       char **argv, char **colNames) {
        auto *ret = reinterpret_cast<std::vector<Row> *>(result_);
        ret->push_back(Row());
        for (int i = 0; i < argc; i++) {
            if (argv[i] != nullptr) {
                ret->back()
                        .push_back(std::make_pair(colNames[i],
                                                  Value(new std::string(argv[i]))));
            } else {
                ret->back()
                        .push_back(std::make_pair(colNames[i], Value(nullptr)));
            }
        }
        return SQLITE_OK;
    };
    std::vector<Row> ret;
    int err = sqlite3_exec(db, sql.c_str(), callback, &ret, &errmsg);
    if (err != SQLITE_OK) {
        std::string errstr = errmsg;
        sqlite3_free(errmsg);
        errmsg = nullptr;
        throw std::runtime_error(errstr);
    }
    return ret;
}



//
//
//    int exit = 0;
//    sqlite3* DB;
//

//
//    exit = sqlite3_open(sqlite_database.string().c_str(), &DB);
//
//
//    if (exit) {
//        std::cerr << "Error open sqlite DB " << sqlite_database << ": " << sqlite3_errmsg(DB) << std::endl;
//        return -1;
//    }
//    else
//        if(!db_exists) {
//            std::cout << "Created new database!" << std::endl;
//
//            char* messageError;
//            exit = sqlite3_exec(DB,
//                    (patient_sql + studies_sql + series_sql + images_sql).c_str(), NULL, 0, &messageError);
//
//            if (exit != SQLITE_OK) {
//                std::cerr << "Error creating tables." << std::endl;
//                sqlite3_free(messageError);
//                return 1;
//            }
//        //            else
//        //                std::cout << "Table created Successfully" << std::endl;
//        //
//        //
//        }
//        else {
//            std::cout << "Opened database." << std::endl;
//        }


//int main() {
//    std::cout << "=== Testing ===" << std::endl;
//    SQLite3 db(":memory:");
//    std::vector<SQLite3::Row> ret;
//    db.exec("CREATE TABLE test ( "
//            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
//            "name TEXT, "
//            "price INTEGER, "
//            "weight REAL, "
//            "createdAt DATE DEFAULT CURRENT_TIMESTAMP);");
//    db.exec("INSERT INTO test (name, price, weight) VALUES "
//            "('apple', 100, 310.5);");
//    db.exec("INSERT INTO test (name, price, weight) VALUES "
//            "('banana', 200, 135.8);");
//    db.exec("INSERT INTO test (name, price, weight) VALUES "
//            "('orange', 130, 222.2);");
//    ret = db.exec("SELECT * FROM test "
//                  "WHERE 100 <= price AND price <= 150 "
//                  "ORDER BY id DESC");
//    std::cout << "===" << std::endl;
//    for (const auto &row : ret) {
//        // use as map
//        std::map<std::string, SQLite3::Value> rowMap(row.begin(), row.end());
//        std::cout << "[" << *rowMap["name"] << "]" << std::endl;
//
//        // iteration
//        for (const auto &col : row) {
//            std::cout << col.first << ':' << *col.second << std::endl;
//        }
//        std::cout << "===" << std::endl;
//    }
//
//    // note: colunm name may be duplicate
//    ret = db.exec("SELECT * FROM test test1 CROSS JOIN test test2 "
//                  "WHERE 100 <= test1.price AND test1.price <= 150 "
//                  "ORDER BY test1.id DESC");
//    for (const auto &row : ret) {
//        for (const auto &col : row) {
//            std::cout << col.first << ':' << *col.second << std::endl;
//        }
//        std::cout << "===" << std::endl;
//    }
//}