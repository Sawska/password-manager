#include "PasswordManager.h"
#include <pqxx/pqxx>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <stdexcept>
#include "env_parser.h"

using namespace std;

std::string getConnectionString() {
    auto env_vars = load_env("/Users/alexanderkorzh/Desktop/coding/password_manager/.env");

    auto dbname_it = env_vars.find("DATABASE_NAME");
    auto user_it = env_vars.find("DATABASE_USER");
    auto password_it = env_vars.find("DATABASE_PASSWORD");
    auto host_it = env_vars.find("DATABASE_HOST");
    auto port_it = env_vars.find("DATABASE_PORT");

    if (dbname_it == env_vars.end() ||
        user_it == env_vars.end() ||
        password_it == env_vars.end() ||
        host_it == env_vars.end() ||
        port_it == env_vars.end()) {
        throw std::runtime_error("Environment variables for database connection are not set.");
    }

    return "dbname=" + dbname_it->second +
           " user=" + user_it->second +
           " password=" + password_it->second +
           " hostaddr=" + host_it->second +
           " port=" + port_it->second;
}

PasswordManager::PasswordManager()
    : c(getConnectionString())
{
    try {
        pqxx::work tx{c};
        tx.exec("CREATE TABLE IF NOT EXISTS passwords ("
                "domain_name VARCHAR(255) NOT NULL, "
                "login VARCHAR(255) NOT NULL, "
                "password VARCHAR(255) NOT NULL, "
                "PRIMARY KEY (domain_name, login));");
        tx.commit();
        cout << "Table 'passwords' ensured to exist." << endl;

        c.prepare("insert_password", "INSERT INTO passwords (domain_name, login, password) VALUES ($1, $2, $3);");
        c.prepare("delete_password", "DELETE FROM passwords WHERE domain_name = $1;");
        c.prepare("update_password", "UPDATE passwords SET password = $1 WHERE domain_name = $2 AND login = $3;");
        c.prepare("select_password", "SELECT domain_name, login, password FROM passwords WHERE domain_name = $1 AND login = $2 AND password = $3;");

    } catch(const pqxx::sql_error &e) {
        cerr << "SQL error: " << e.what() << endl;
    } catch(const std::exception &e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void PasswordManager::add_password(const std::string& domain_name, const std::string& login, const std::string& password) {
    try {
        pqxx::work tx{c};
        tx.exec_prepared("insert_password", domain_name, login, password);
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
        tx.exec_prepared("delete_password", domain_name);
        tx.commit();
    } catch(const pqxx::sql_error &e) {
        cerr << "SQL error: " << e.what() << endl;
    } catch(const std::exception &e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void PasswordManager::update_password(const PasswordUnit& unit) {
    try {
        
        std::string domain_name = boost::locale::conv::to_utf<char>(unit.domain_name, "UTF-8");
        std::string login = boost::locale::conv::to_utf<char>(unit.login, "UTF-8");
        std::string password = boost::locale::conv::to_utf<char>(unit.getPassword(), "UTF-8");

        pqxx::work tx{c};

        
        std::cout << "Updating password for domain: " << domain_name
                  << ", login: " << login
                  << ", password: " << password << std::endl;

        tx.exec_prepared("update_password", password, domain_name, login);
        tx.commit();
    } catch(const pqxx::sql_error &e) {
        std::cerr << "SQL error: Failure during 'update_password': " << e.what() << std::endl;
        std::cerr << "SQL error: " << e.query() << std::endl;
    } catch(const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
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
        pqxx::work tx(c);
        pqxx::result res = tx.exec_prepared("select_password", domain_name, login, password);
        tx.commit();
        if (!res.empty()) {
            unit.domain_name = res[0][0].c_str();
            unit.login = res[0][1].c_str();
            unit.password = res[0][2].c_str();
        }
    } catch(const pqxx::sql_error &e) {
        std::cerr << "SQL error: " << e.what() << std::endl;
    } catch(const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return unit;
}
