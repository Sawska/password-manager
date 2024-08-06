#include "PasswordManager.h"
#include "PasswordUnit.h"
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/color.hpp"
#include <iostream>
#include "crow.h"
#include "env_parser.h"
#include <thread>

const std::string ENV_FILE_PATH = "/Users/alexanderkorzh/Desktop/coding/password_manager/.env";

std::string SECRET_PASSWORD;

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
    PasswordUnit pas_unit(domain_name, login, password);
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
    std::cout << "Domain: " << pu.domain_name << ", Login: " << pu.login << ", Password: " << pu.password << std::endl;
}

void showAllPasswords(PasswordManager& pm) {
    std::vector<PasswordUnit> units = pm.select_all();
    for (size_t i = 0; i < units.size(); ++i) {
        const PasswordUnit& unit = units[i];
        std::cout << std::to_string(i + 1) << " Domain: " << unit.domain_name 
                  << ", Login: " << unit.login 
                  << ", Password: " << unit.getPassword() << std::endl;
    }
}

void updateSecretPassword() {
    std::string new_password;
    std::cout << Term::color_fg(Term::Color::Name::Red) << "Enter new secret password" << Term::color_fg(Term::Color::Name::Default) << std::endl;
    std::cin >> new_password;

    auto env_vars = load_env(ENV_FILE_PATH);
    env_vars["SECRET_PASSWORD"] = new_password;

    save_env(ENV_FILE_PATH, env_vars);

    SECRET_PASSWORD = new_password;
    std::cout << "Secret password updated successfully!" << std::endl;
}

void runServer() {
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
}

int main() {
    auto env_vars = load_env(ENV_FILE_PATH);
    SECRET_PASSWORD = env_vars["SECRET_PASSWORD"];

    Term::Terminal term;

    std::cout << Term::color_fg(Term::Color::Name::Red) << "Welcome to Password Manager" << Term::color_fg(Term::Color::Name::Default) << std::endl;

    std::string password;

    if (SECRET_PASSWORD.empty()) {
        std::cout << Term::color_fg(Term::Color::Name::Red) << "Please create a secret password" << Term::color_fg(Term::Color::Name::Default) << std::endl;
        std::cin >> SECRET_PASSWORD;
    }

    while (SECRET_PASSWORD != password) {
        std::cout << Term::color_fg(Term::Color::Name::Red) << "Please enter your secret password" << Term::color_fg(Term::Color::Name::Default) << std::endl;
        std::cin >> password;
    }

    PasswordManager pm;
    std::thread server_thread;

    int option = 0;
    while (option != 8) {
        std::cout << Term::color_fg(Term::Color::Name::Blue) << "Please enter an option" << Term::color_fg(Term::Color::Name::Default) << std::endl;
        std::cout << "1) Show password by criteria" << std::endl;
        std::cout << "2) Show all passwords" << std::endl;
        std::cout << "3) Add password" << std::endl;
        std::cout << "4) Update password" << std::endl;
        std::cout << "5) Update secret password" << std::endl;
        std::cout << "6) Remove password" << std::endl;
        std::cout << "7) Run server" << std::endl;
        std::cout << "8) Exit" << std::endl;

        std::cin >> option;

        switch (option) {
            case 1:
                showPasswords(pm);
                break;
            case 2:
                showAllPasswords(pm);
                break;
            case 3:
                addPassword(pm);
                break;
            case 4:
                updatePassword(pm);
                break;
            case 5:
                updateSecretPassword();
                break;
            case 6:
                removePassword(pm);
                break;
            case 7:
                server_thread = std::thread(runServer);
                server_thread.detach();
                std::cout << "Server running on port 8080" << std::endl;
                break;
            case 8:
                std::cout << "Exiting..." << std::endl;
                break;
            default:
                std::cout << "Invalid option. Please try again." << std::endl;
                option = 0;
                break;
        }
    }

    return 0;
}
