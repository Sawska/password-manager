#ifndef ENV_PARSER_H
#define ENV_PARSER_H

#include <string>
#include <unordered_map>

std::unordered_map<std::string, std::string> load_env(const std::string& file_path);
void save_env(const std::string& file_path, const std::unordered_map<std::string, std::string>& env_vars);

#endif // ENV_PARSER_H
