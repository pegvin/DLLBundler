#ifndef _BUNDLER_UTILS_H_INCLUDED
#define _BUNDLER_UTILS_H_INCLUDED 1
#pragma once

#include <string>
#include <vector>

using String = std::string;

template<typename Type>
using Vector = std::vector<Type>;

// trim from both ends (in place)
void trim(String &s);

// lower-cases the COPY of the original string 's' & returns it
String StringToLowerCase(String& s);

// Split "stringToSplit" By "delimiter" And Put Into A Vector, Return The Vector
Vector<String> StringSplit(String stringToSplit, String delimiter);

// checks if a "path" exists
bool PathExists(String& path);

// checks if a file at "path" exists
bool PathIsFile(String& path);

// executes a command and returns the stdout
String ExecuteShellCmd(const char* cmd);

#endif // _BUNDLER_UTILS_H_INCLUDED
