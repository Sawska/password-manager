#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <string>
#include <vector>
#include <pqxx/pqxx>
#include "PasswordUnit.h"
#include <boost/locale.hpp>


std::string getConnectionString();

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
    
};

#endif // PASSWORDMANAGER_H
