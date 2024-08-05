#include "PasswordManager.h"
#include "PasswordUnit.h"
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/color.hpp"
#include <iostream>
#include "crow.h"

std::string SECRET_PASSWORD = "";

void addPassword(PasswordManager& pm) {
    std::string domain_name, login, password;
    std::cout << "Enter domain name: ";
    std::cin >> domain_name;
    std::cout << "Enter login: ";
    std::cin >> login;
    std::cout << "Enter password: ";
    std::cin >> password;
    pm.add_password(domain_name, login, password);
    std::cout << "Password added successfully!" << std::endl;
}

void updatePassword(PasswordManager& pm) {
    std::string domain_name, login, password;
    std::cout << "Enter domain name: ";
    std::cin >> domain_name;
    std::cout << "Enter login: ";
    std::cin >> login;
    std::cout << "Enter new password: ";
    std::cin >> password;
    PasswordUnit pas_unit(domain_name,login,password);
    pm.update_password(pas_unit);
    std::cout << "Password updated successfully!" << std::endl;
}


void removePassword(PasswordManager& pm) {
    std::string domain_name;
    std::cout << "Enter domain name: ";
    std::cin >> domain_name;
    pm.remove_password_by_domain(domain_name);
    std::cout << "Password removed successfully!" << std::endl;
}

void showPasswords(PasswordManager& pm) {
    std::string domain_name, login, password;
    std::cout << "Enter domain name (leave empty to skip): ";
    std::cin >> domain_name;
    std::cout << "Enter login (leave empty to skip): ";
    std::cin >> login;
    std::cout << "Enter password (leave empty to skip): ";
    std::cin >> password;
    PasswordUnit pu = pm.select_from_passwords(domain_name, login, password);
    std::cout << "Domain: " << pu.domain_name << ", Login: " << pu.login << ", Password: " << pu.getPassword() << std::endl;
}

void showAllPasswords(PasswordManager& pm) {
    vector<PasswordUnit> units = pm.select_all();

    for (int i = 0; i < units.size(); ++i) {
        PasswordUnit unit = units[i];
        std::cout << std::to_string(i + 1) << " Domain: " << unit.domain_name 
                  << ", Login: " << unit.login 
                  << ", Password: " << unit.getPassword() << std::endl;
    }
}



void updateSecretPassword() {
    std::string new_password;
    std::cout << "Enter new secret password: ";
    std::cin >> new_password;
    SECRET_PASSWORD = new_password;
    std::cout << "Secret password updated successfully!" << std::endl;
}

int main() {
    Term::Terminal term;

    std::cout << Term::color_fg(Term::Color::Name::Red) << "Welcome to Password Manager" << Term::color_fg(Term::Color::Name::Default) << std::endl;
    // std::cout <<  "Welcome to Password Manager" << std::endl;

    std::string password;

    if (SECRET_PASSWORD.empty()) {
        std::cout << Term::color_fg(Term::Color::Name::Red) << "Please create a secret password" << Term::color_fg(Term::Color::Name::Default) << std::endl;
        // std::cout << "Please create a secret password"  << std::endl;
        std::cin >> SECRET_PASSWORD;
    }

    while (SECRET_PASSWORD != password) {
        std::cout << Term::color_fg(Term::Color::Name::Red) << "Please enter your secret password" << Term::color_fg(Term::Color::Name::Default) << std::endl;
        // std::cout << "Please enter your secret password"  << std::endl;
        std::cin >> password;
    }

    PasswordManager pm;

    int option = 0;
    while (option != 6) {
        std::cout << Term::color_fg(Term::Color::Name::Blue) << "Please enter an option" << Term::color_fg(Term::Color::Name::Default) << std::endl;
        std::cout << "Please enter an option" <<  std::endl;
        std::cout << "1) Show passwords" << std::endl;
        std::cout << "2) Add password" << std::endl;
        std::cout << "3) Update password" << std::endl;
        std::cout << "4) Update secret password" << std::endl;
        std::cout << "5) Remove password" << std::endl;
        std::cout << "6) Exit" << std::endl;

        std::cin >> option;

        switch (option) {
            case 1:
                showPasswords(pm);
                break;
            case 2:
                addPassword(pm);
                break;
            case 3:
                updatePassword(pm);
                break;
            case 4:
                updateSecretPassword();
                break;
            case 5:
                removePassword(pm);
                break;
            case 6:
                std::cout << "Exiting..." << std::endl;
                break;
            default:
                std::cout << "Invalid option. Please try again." << std::endl;
                break;
        }
    }

    crow::SimpleApp app;

    crow::mustache::set_base(".");

    CROW_ROUTE(app, "/store_form_data")
    .methods("POST"_method)
    ([&](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x)
            return crow::response(400);

        std::string domain_name = x["domain"].s();
        std::string login = x["login"].s();
        std::string password = x["password"].s();

        PasswordManager pm;
        PasswordUnit unit(domain_name, login, password);
        
        
        pm.add_password(domain_name, login, password);
        
        crow::json::wvalue res;
        res["status"] = "success";
        return crow::response(res);
    });

    app.port(8080).multithreaded().run();

    return 0;
}
