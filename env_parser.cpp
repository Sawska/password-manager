#include "env_parser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::unordered_map<std::string, std::string> load_env(const std::string& file_path) {
    std::unordered_map<std::string, std::string> env_vars;
    std::ifstream file(file_path);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open .env file");
    }

    std::string line;
    while (std::getline(file, line)) {
        
        if (line.empty() || line[0] == '#') continue;

        
        std::istringstream iss(line);
        std::string key, value;

        if (std::getline(std::getline(iss, key, '=') >> std::ws, value)) {
            env_vars[key] = value;
        }
    }

    file.close();
    return env_vars;
}

void save_env(const std::string& file_path, const std::unordered_map<std::string, std::string>& env_vars) {
    std::ofstream file(file_path);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open .env file for writing");
    }

    for (const auto& pair : env_vars) {
        file << pair.first << "=" << pair.second << "\n";
    }

    file.close();
}
