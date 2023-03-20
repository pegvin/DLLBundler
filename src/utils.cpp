#include <cstdio>
#include <unistd.h>
#include <memory>
#include <stdexcept>
#include <array>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <filesystem>

#include "utils.h"

namespace fs = std::filesystem;

static void ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
	}));
}

static void rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}

void trim(std::string &s) {
	rtrim(s);
	ltrim(s);
}

std::string& StringLowerCaseRef(std::string& s) {
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
	return s;
}

std::string StringLowerCaseCopy(std::string& s) {
	auto newStr = s;
	std::transform(newStr.begin(), newStr.end(), newStr.begin(), [](unsigned char c){ return std::tolower(c); });
	return newStr;
}

Vector<String> StringSplit(String str, String token){
	Vector<String> result;
	while (str.size()) {
		String::size_type index = str.find(token);
		if (index != String::npos){
			result.push_back(str.substr(0, index));
			str = str.substr(index + token.size());
			if (str.size() == 0) result.push_back(str);
		} else{
			result.push_back(str);
			break;
		}
	}
	return result;
}

bool PathExists(String& path) {
	return fs::exists(fs::path(path));
}

bool PathExists(fs::path& path) {
	return fs::exists(path);
}

bool PathIsFile(String& _path) {
	auto path = fs::path(_path);
	return PathExists(path) && (!fs::is_directory(path));
}

std::string ExecuteShellCmd(const char* cmd) {
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}
	return result;
}

