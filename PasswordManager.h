#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <string>
#include <vector>
#include <pqxx/pqxx>
#include "PasswordUnit.h"

const std::string CONNECTION_STRING = "dbname=passwords user=postgres password=AlexLinov hostaddr=localhost port=5432";

class PasswordManager {
public:
    PasswordManager(); 

    void add_password(const std::string& domain_name, const std::string& login, const std::string& password);
    void remove_password_by_domain(const std::string& domain_name);
    void update_password(const PasswordUnit& unit);
    std::vector<PasswordUnit> select_all();
    PasswordUnit select_from_passwords(const std::string& domain_name = "", const std::string& login = "", const std::string& password = "");

private:
    pqxx::connection c;
    pqxx::work tx;
};

#endif // PASSWORDMANAGER_H
