#include "PasswordManager.h"
#include <mysqlx/xdevapi.h>
#include <iostream>

using namespace std;
using namespace mysqlx;


PasswordManager::PasswordManager()
    : sess(URL)
{
    try {
        Schema db  = sess.getSchema("password_manager_db");
        sess.sql("CREATE TABLE IF NOT EXISTS passwords ("
        "domain_name VARCHAR(255) NOT NULL, "
        "login VARCHAR(255) NOT NULL, "
        "password VARCHAR(255) NOT NULL, "
        "PRIMARY KEY (domain_name, login))").execute();

        cout << "Table 'passwords' ensure to exist," << endl;
    } catch(const mysqlx::Error &err) {
        cerr << "Error:" << err.what() << endl;
    }
}

void PasswordManager::add_password(const std::string& domain_name, const std::string& login, const std::string& password)
{
    try {
        Schema db = sess.getSchema("password_manager_db"); 
        Table passwords = db.getTable("passwords");

        passwords.insert("domain_name", "login", "password")
            .values(domain_name, login, password)
            .execute();

        std::cout << "Password added successfully." << endl;
    }
    catch (const mysqlx::Error &err) {
        cerr << "Error: " << err.what() << endl;
    }
}

vector<PasswordUnit> PasswordManager::select_all() {
    vector<PasswordUnit> result;
    try {
        Schema db = sess.getSchema("password_manager_db");
        Table passwords = db.getTable("passwords");

        RowResult rows = passwords.select("domain_name", "login", "password").execute();

        for (Row row : rows) {
            std::string domain_name = row[0].get<std::string>();
            std::string login = row[1].get<std::string>();
            std::string password = row[2].get<std::string>();
            result.emplace_back(domain_name, login, password);
        }

    } catch (const mysqlx::Error &err) {
        cerr << "Error: " << err.what() << endl;
    }
    return result;
}

void PasswordManager::remove_password_by_domain(const std::string& domain_name)
{
    try {
        Schema db = sess.getSchema("password_manager_db"); 
        Table passwords = db.getTable("passwords"); 

        passwords.remove()
            .where("domain_name = :domain_name")
            .bind("domain_name", domain_name)
            .execute();

        std::cout << "Password(s) removed successfully for domain: " << domain_name << endl;
    }
    catch (const mysqlx::Error &err) {
        cerr << "Error: " << err.what() << endl;
    }
}

void PasswordManager::update_password(PasswordUnit unit)
{
    try {
        Schema db = sess.getSchema("password_manager_db"); 
        Table passwords = db.getTable("passwords"); 
        std::string query = 
            "UPDATE passwords SET password = :new_password WHERE "
            "(domain_name = NULLIF(:domain_name, '') OR :domain_name = '') AND "
            "(login = NULLIF(:login, '') OR :login = '')";

        passwords.update()
            .set("password", unit.password)
            .where("domain_name = :domain_name AND login = :login")
            .bind("domain_name", unit.domain_name)
            .bind("login", unit.login)
            .execute();

        cout << "Password updated successfully for login: " << unit.login << " in domain: " << unit.domain_name << endl;
    }
    catch (const mysqlx::Error &err) {
        cerr << "Error: " << err.what() << endl;
    }
}

PasswordUnit PasswordManager::select_from_passwords(const std::string& domain_name, const std::string& login, const std::string& password)
{
    PasswordUnit result;
    try {
        Schema db = sess.getSchema("password_manager_db"); 
        Table passwords = db.getTable("passwords"); 

        
        std::string query = 
            "SELECT domain_name, login, password FROM passwords WHERE "
            "(domain_name = NULLIF(:domain_name, '') OR :domain_name = '') AND "
            "(login = NULLIF(:login, '') OR :login = '') AND "
            "(password = NULLIF(:password, '') OR :password = '')";

        RowResult rows = sess.sql(query)
            .bind("domain_name", domain_name)
            .bind("login", login)
            .bind("password", password)
            .execute();

        if (rows.count() > 0) {
            Row row = rows.fetchOne();
            result.domain_name = row[0].get<std::string>();
            result.login = row[1].get<std::string>();
            result.password = row[2].get<std::string>();
        }

        cout << "Query executed successfully." << endl;
    }
    catch (const mysqlx::Error &err) {
        cerr << "Error: " << err.what() << endl;
    }

    return result;
}
