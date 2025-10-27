#include "apgnFileSys.hpp"
#include <exception>

namespace apgnFileSys {
void deleteFile(const std::string &filename) {
  try {
    if (std::filesystem::remove(filename)) {
      std::cout << "...\n";
    } else {
      std::cout << "file " << filename << " not found, no-op delete.\n";
    }
  } catch (const std::filesystem::filesystem_error &err) {
    throw err;
  }
}

std::string getExecpath() {
#if defined(__linux__)
  char result[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  const char *path;
  if (count != -1) {
    path = dirname(result);
  }
  return std::filesystem::path(path).string();
#elif defined(_WIN32)
  WCHAR path[MAX_PATH];
  GetModuleFileNameW(NULL, path, MAX_PATH);

  std::wstring string_to_convert = path;

  // setup converter
  using convert_type = std::codecvt_utf8<wchar_t>;
  std::wstring_convert<convert_type, wchar_t> converter;

  // use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
  std::string converted_str = converter.to_bytes(string_to_convert);

  return std::filesystem::path(
             converted_str.substr(0, converted_str.size() - 9))
      .string();
#endif
}
} // namespace apgnFileSys
