#ifndef _BUNDLER_UTILS_H_INCLUDED
#define _BUNDLER_UTILS_H_INCLUDED 1
#pragma once

#include <string>
#include <vector>

// trim from both ends (in place)
void trim(std::string &s);

// lower-cases the original string 's' & returns it
std::string& StringLowerCaseRef(std::string& s);

// lower-cases the COPY of the original string 's' & returns it
std::string StringLowerCaseCopy(std::string& s);

// Split "stringToSplit" By "delimiter" And Put Into A Vector, Return The Vector
std::vector<std::string> StringSplit(std::string stringToSplit, std::string delimiter);

// checks if a file at "path" exists
bool doesFileExist(std::string& path);

// executes a command and returns the stdout
std::string ExecuteShellCmd(const char* cmd);

#endif // _BUNDLER_UTILS_H_INCLUDED
