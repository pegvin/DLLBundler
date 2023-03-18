#include <cstdio>
#include <unistd.h>
#include <memory>
#include <stdexcept>
#include <array>
#include <algorithm> 
#include <cctype>
#include <locale>

#include "utils.h"

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

std::vector<std::string> StringSplit(std::string s, std::string delimiter) {
	size_t pos = 0;
	std::string token;
	std::vector<std::string> arr;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		arr.emplace_back(token);
		s.erase(0, pos + delimiter.length());
	}
	return arr;
}

bool doesFileExist(std::string& path) {
	int res = access(path.c_str(), R_OK);
	if (res < 0) return false;
	return true;
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


