#include "PasswordManager.h"
#include <pqxx/pqxx>
#include <iostream>

using namespace std;

PasswordManager::PasswordManager()
    : c(CONNECTION_STRING), tx(c)
{
    try {
        pqxx::work tx{c};
        tx.exec("CREATE TABLE IF NOT EXISTS passwords ("
                "domain_name VARCHAR(255) NOT NULL, "
                "login VARCHAR(255) NOT NULL, "
                "password VARCHAR(255) NOT NULL, "
                "PRIMARY KEY (domain_name, login));");
        tx.commit();
        cout << "Table 'passwords' ensured to exist," << endl;
    } catch(const pqxx::sql_error &e) {
        cerr << "SQL error: " << e.what() << endl;
    } catch(const std::exception &e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void PasswordManager::add_password(const std::string& domain_name, const std::string& login, const std::string& password) {
    try {
        pqxx::work tx{c};
        tx.exec_prepared("INSERT INTO passwords (domain_name, login, password) VALUES ($1, $2, $3);",
                         domain_name, login, password);
        tx.commit();
    } catch(const pqxx::sql_error &e) {
        cerr << "SQL error: " << e.what() << endl;
    } catch(const std::exception &e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void PasswordManager::remove_password_by_domain(const std::string& domain_name) {
    try {
        pqxx::work tx{c};
        tx.exec_prepared("DELETE FROM passwords WHERE domain_name = $1;", domain_name);
        tx.commit();
    } catch(const pqxx::sql_error &e) {
        cerr << "SQL error: " << e.what() << endl;
    } catch(const std::exception &e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void PasswordManager::update_password(const PasswordUnit& unit) {
    try {
        pqxx::work tx{c};
        tx.exec_prepared("UPDATE passwords SET password = $1 WHERE domain_name = $2 AND login = $3;",
                         unit.password, unit.domain_name, unit.login);
        tx.commit();
    } catch(const pqxx::sql_error &e) {
        cerr << "SQL error: " << e.what() << endl;
    } catch(const std::exception &e) {
        cerr << "Error: " << e.what() << endl;
    }
}

std::vector<PasswordUnit> PasswordManager::select_all() {
    std::vector<PasswordUnit> passwords;
    try {
        pqxx::nontransaction ntx{c};
        pqxx::result res = ntx.exec("SELECT domain_name, login, password FROM passwords;");
        for (const auto& row : res) {
            PasswordUnit unit;
            unit.domain_name = row[0].c_str();
            unit.login = row[1].c_str();
            unit.password = row[2].c_str();
            passwords.push_back(unit);
        }
    } catch(const pqxx::sql_error &e) {
        cerr << "SQL error: " << e.what() << endl;
    } catch(const std::exception &e) {
        cerr << "Error: " << e.what() << endl;
    }
    return passwords;
}

PasswordUnit PasswordManager::select_from_passwords(const std::string& domain_name, const std::string& login, const std::string& password) {
    PasswordUnit unit;
    try {
        pqxx::nontransaction ntx{c};
        pqxx::result res = ntx.exec_prepared("SELECT domain_name, login, password FROM passwords WHERE domain_name = $1 AND login = $2 AND password = $3;",
                                             domain_name, login, password);
        if (!res.empty()) {
            unit.domain_name = res[0][0].c_str();
            unit.login = res[0][1].c_str();
            unit.password = res[0][2].c_str();
        }
    } catch(const pqxx::sql_error &e) {
        cerr << "SQL error: " << e.what() << endl;
    } catch(const std::exception &e) {
        cerr << "Error: " << e.what() << endl;
    }
    return unit;
}
