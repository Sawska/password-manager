#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <string>
#include <mysqlx/xdevapi.h>
#include "PasswordUnit.h"

const std::string URL = "mysqlx://root@127.0.0.1";

using namespace mysqlx;

class PasswordManager {
public:
    PasswordManager(); 

    void add_password(const std::string& domain_name, const std::string& login, const std::string& password);
    void remove_password_by_domain(const std::string& domain_name);
    void update_password(PasswordUnit unit);
    std::vector<PasswordUnit> select_all();
    PasswordUnit select_from_passwords(const std::string& domain_name = "", const std::string& login = "", const std::string& password = "");

private:
    Session sess;
};

#endif // PASSWORDMANAGER_H
