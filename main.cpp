#include "PasswordManager.h"
#include <regex>
#include "PasswordUnit.h"
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/color.hpp"
#include <iostream>
#include "crow.h"
#include "env_parser.h"
#include <thread>

const std::string ENV_FILE_PATH = "/Users/alexanderkorzh/Desktop/coding/password_manager/.env";

std::string SECRET_PASSWORD;




bool isValidPassword(const std::string& password) {
    
    std::regex valid_chars("[ -~]+"); 
    return std::regex_match(password, valid_chars);
}

void addPassword(PasswordManager& pm) {
    std::string domain_name, login, password;
    
    std::cout << Term::color_fg(Term::Color::Name::Red) << "Enter domain name: " << Term::color_fg(Term::Color::Name::Default) << std::endl;
    std::cin >> domain_name;
    
    std::cout << Term::color_fg(Term::Color::Name::Red) << "Enter login: " << Term::color_fg(Term::Color::Name::Default) << std::endl;
    std::cin >> login;
    
    std::cout << Term::color_fg(Term::Color::Name::Red) << "Enter password: " << Term::color_fg(Term::Color::Name::Default) << std::endl;
    std::cin >> password;

    
    if (!isValidPassword(password)) {
        std::cout << Term::color_fg(Term::Color::Name::Red) << "Invalid password! Password contains invalid characters." << Term::color_fg(Term::Color::Name::Default) << std::endl;
        return;
    }

    
    pm.add_password(domain_name, login, password);
    std::cout << Term::color_fg(Term::Color::Name::Blue) << "Password added successfully!" << Term::color_fg(Term::Color::Name::Default) << std::endl;
}


void updatePassword(PasswordManager& pm) {
    std::string domain_name, login, password;
    std::cout << Term::color_fg(Term::Color::Name::Red) << "Enter domain name: " << Term::color_fg(Term::Color::Name::Default) << std::endl;
    std::cin >> domain_name;
    std::cout << Term::color_fg(Term::Color::Name::Red) << "Enter login: " << Term::color_fg(Term::Color::Name::Default) << std::endl;
    std::cin >> login;
    std::cout << Term::color_fg(Term::Color::Name::Red) << "Enter new password: " << Term::color_fg(Term::Color::Name::Default) << std::endl;
    std::cin >> password;
    PasswordUnit pas_unit(domain_name, login, password);
    pm.update_password(pas_unit);
    std::cout <<  Term::color_fg(Term::Color::Name::Blue) << "Password updated successfully!"  << Term::color_fg(Term::Color::Name::Default) << std::endl;
}

void removePassword(PasswordManager& pm) {
    std::string domain_name, login, password;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::cout << Term::color_fg(Term::Color::Name::Red) << "Enter domain name (leave empty to skip): " << Term::color_fg(Term::Color::Name::Default) << std::endl;
    std::getline(std::cin, domain_name);
    
    std::cout << Term::color_fg(Term::Color::Name::Red) << "Enter login (leave empty to skip): " << Term::color_fg(Term::Color::Name::Default) << std::endl;
    std::getline(std::cin, login);
    
    std::cout << Term::color_fg(Term::Color::Name::Red) << "Enter password (leave empty to skip): " << Term::color_fg(Term::Color::Name::Default) << std::endl;
    std::getline(std::cin, password);
    
    if (domain_name.empty() && login.empty() && password.empty()) {
        std::cout << Term::color_fg(Term::Color::Name::Red) << "You need to enter at least one value" << Term::color_fg(Term::Color::Name::Default) << std::endl;
        return;
    }
    pm.remove_password(domain_name,login,password);
    std::cout << Term::color_fg(Term::Color::Name::Blue) << "Password removed successfully!" << Term::color_fg(Term::Color::Name::Default) << std::endl;
}

void showPasswords(PasswordManager& pm) {
    std::string domain_name, login, password;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::cout << Term::color_fg(Term::Color::Name::Red) << "Enter domain name (leave empty to skip): " << Term::color_fg(Term::Color::Name::Default) << std::endl;
    std::getline(std::cin, domain_name);
    
    std::cout << Term::color_fg(Term::Color::Name::Red) << "Enter login (leave empty to skip): " << Term::color_fg(Term::Color::Name::Default) << std::endl;
    std::getline(std::cin, login);
    
    std::cout << Term::color_fg(Term::Color::Name::Red) << "Enter password (leave empty to skip): " << Term::color_fg(Term::Color::Name::Default) << std::endl;
    std::getline(std::cin, password);
    
    if (domain_name.empty() && login.empty() && password.empty()) {
        std::cout << Term::color_fg(Term::Color::Name::Red) << "You need to enter at least one value" << Term::color_fg(Term::Color::Name::Default) << std::endl;
        return;
    }

    PasswordUnit pu = pm.select_from_passwords(domain_name, login, password);

    try {
        std::cout << Term::color_fg(Term::Color::Name::Blue) << "Domain: " << pu.domain_name 
                  << ", Login: " << pu.login 
                  << ", Password: " << pu.getPassword() 
                  << Term::color_fg(Term::Color::Name::Default) << std::endl;
    } catch (const std::exception& e) {
        std::cerr << Term::color_fg(Term::Color::Name::Red) << "Error: " << e.what() << Term::color_fg(Term::Color::Name::Default) << std::endl;
    }
}




void showAllPasswords(PasswordManager& pm) {
    std::vector<PasswordUnit> units = pm.select_all();
    for (size_t i = 0; i < units.size(); ++i) {
        const PasswordUnit& unit = units[i];
        std::cout << Term::color_fg(Term::Color::Name::Blue) << std::to_string(i + 1) << " Domain: " << unit.domain_name 
                  << ", Login: " << unit.login 
                  << ", Password: " << unit.getPassword() << Term::color_fg(Term::Color::Name::Default) << std::endl;
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
    std::cout << Term::color_fg(Term::Color::Name::Green) << "Secret password updated successfully!"  << Term::color_fg(Term::Color::Name::Default) << std::endl;
}

void runServer(PasswordManager& pm) {
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

    PasswordUnit unit(domain_name, login, password);
    // std::string decryptedPassword = unit.getPassword();
    // std::cout << "Decrypted Password: " << decryptedPassword << std::endl;
    // std::cout << "Decrypted Password: " << unit.password << std::endl;
    pm.add_password(domain_name,login,password);

    

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

    std::cout << Term::color_fg(Term::Color::Name::Blue) << "Welcome to Password Manager" << Term::color_fg(Term::Color::Name::Default) << std::endl;

    std::string password;

    
    if (SECRET_PASSWORD.empty()) {
        std::cout << Term::color_fg(Term::Color::Name::Red) << "Please create a secret password" << Term::color_fg(Term::Color::Name::Default) << std::endl;
        std::cin >> SECRET_PASSWORD;
    }

    
    while (SECRET_PASSWORD != password) {
        std::cout << Term::color_fg(Term::Color::Name::Blue) << "Please enter your secret password" << Term::color_fg(Term::Color::Name::Default) << std::endl;
        std::cin >> password;
        if(SECRET_PASSWORD != password) {
            std::cout << Term::color_fg(Term::Color::Name::Red) << "This is not right password" << Term::color_fg(Term::Color::Name::Default) << std::endl;
        }
    }

    PasswordManager pm;
    std::thread server_thread;

    int option = 0;
    while (true) {
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

        if (std::cin.fail()) {
            std::cin.clear(); 
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            std::cout << Term::color_fg(Term::Color::Name::Red) << "Invalid input. Please enter a number between 1 and 8." << Term::color_fg(Term::Color::Name::Default) << std::endl;
            continue;
        }

        try {
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
                    server_thread = std::thread(runServer, std::ref(pm));
                    server_thread.detach();
                    std::cout << Term::color_fg(Term::Color::Name::Green) << "Server running on port 8080" << Term::color_fg(Term::Color::Name::Default) << std::endl;
                    break;

                case 8:
                    std::cout << Term::color_fg(Term::Color::Name::Yellow) << "Exiting..." << Term::color_fg(Term::Color::Name::Default) << std::endl;
                    exit(0);
                    break;
                default:
                    std::cout << Term::color_fg(Term::Color::Name::Red) << "Invalid option. Please try again." << Term::color_fg(Term::Color::Name::Default) << std::endl;
                    break;
            }
        } catch (const std::exception& e) {
            std::cout << Term::color_fg(Term::Color::Name::Red) << "An error occurred: " << e.what() << Term::color_fg(Term::Color::Name::Default) << std::endl;
        }
    }

    return 0;
}