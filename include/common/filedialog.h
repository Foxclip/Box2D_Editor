#include <string>
#include <filesystem>

std::wstring open_file_dialog(const std::filesystem::path& default_folder);
std::wstring open_file_dialog();
std::wstring save_file_dialog(const std::filesystem::path& default_folder);
std::wstring save_file_dialog();
