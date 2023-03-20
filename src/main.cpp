#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include "utils.h"

namespace fs = std::filesystem;

#define HELP_MESSAGE "Usage: dllbundler [-h] [--copy] [--upx] executable\n\n" \
"positional arguments:\n" \
"  executable  EXE or DLL file to bundle dependencies for\n\n" \
"options:\n" \
"  -h, --help  Show this help message and exit\n" \
"  --copy      Copy the DLLs next to the executable\n" \
"  --upx       Run UPX on all the DLLs and EXE (requires --copy)."

bool upx = false;
bool copy = false;
Vector<String> searchDir;
String executable = "";
const Vector<String> blacklist = {
	"advapi32.dll", "kernel32.dll", "msvcrt.dll", "ole32.dll", "user32.dll",
	"ws2_32.dll", "comdlg32.dll", "gdi32.dll", "imm32.dll", "oleaut32.dll",
	"shell32.dll", "winmm.dll", "winspool.drv", "wldap32.dll",
	"ntdll.dll", "d3d9.dll", "mpr.dll", "crypt32.dll", "dnsapi.dll",
	"shlwapi.dll", "version.dll", "iphlpapi.dll", "msimg32.dll", "setupapi.dll",
	"opengl32.dll", "dwmapi.dll", "uxtheme.dll", "secur32.dll", "gdiplus.dll",
	"usp10.dll", "comctl32.dll", "wsock32.dll", "netapi32.dll", "userenv.dll",
	"avicap32.dll", "avrt.dll", "psapi.dll", "mswsock.dll", "glu32.dll",
	"bcrypt.dll", "rpcrt4.dll", "hid.dll", "dbghelp.dll",
	"d3d11.dll", "dxgi.dll", "dwrite.dll"
};

String FindDLLPath(String& dllName, Vector<String>& searchDirectories) {
	for (auto directory : searchDirectories) {
		String dllPath = (fs::path(directory) / fs::path(dllName)).string();
		String dllPathLower = (fs::path(directory) / fs::path(StringToLowerCase(dllName))).string();
		if (PathIsFile(dllPath)) {
			// std::cout << "Found: " << dllPath << std::endl;
			return dllPath;
		} else if (dllPath != dllPathLower && PathIsFile(dllPathLower)) {
			// std::cout << "Found: " << dllPathLower << std::endl;
			return dllPathLower;
		}
	}
	std::cout << "Can't find \"" << dllName << '"' << std::endl;
	return "";
}

Vector<String> GatherRequiredDLLs(String& executable, Vector<String>& foundDLLs) {
	if (executable.empty() || !PathIsFile(executable)) { return {}; }

	Vector<String> ret = { executable };
	String output = ExecuteShellCmd(("objdump -p " + executable + " 2>&1 | grep '\tDLL Name:' 2>&1").c_str());
	Vector<String> dlls = StringSplit(output, "\tDLL Name: ");

	for (auto dllName : dlls) {
		trim(dllName);
		if (dllName.empty()) continue;

		auto dllNameLower = StringToLowerCase(dllName);
		if (
			std::find(blacklist.begin(), blacklist.end(), dllNameLower) != blacklist.end() ||
			std::find(foundDLLs.begin(), foundDLLs.end(), dllNameLower) != foundDLLs.end()
		) continue;

		auto dllPath = FindDLLPath(dllName, searchDir);
		if (!dllPath.empty()) {
			foundDLLs.push_back(dllNameLower);
			auto subDLLs = GatherRequiredDLLs(dllPath, foundDLLs);
			if (!subDLLs.empty()) {
				ret.insert(
					ret.end(),
					std::make_move_iterator(subDLLs.begin()),
					std::make_move_iterator(subDLLs.end())
				);
			}
		}
	}

	return ret;
}

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cout << "Executable not specified!\n" << HELP_MESSAGE << std::endl;
	}

	for (int i = 1; i < argc; ++i) {
		String arg = argv[i];
		if (arg == "-h" || arg == "--help") {
			std::cout << HELP_MESSAGE << std::endl;
			return 0;
		} else if (arg == "--copy") {
			copy = true;
		} else if (arg == "--upx") {
			upx = true;
		} else {
			executable = arg;
		}
	}

	if (!executable.length()) {
		std::cout << "Executable not specified!\n\n" << HELP_MESSAGE << std::endl;
		return 1;
	} else if (upx && !copy) {
		std::cout << "Cannot use UPX compression if --copy isn't enabled!\n\n" << HELP_MESSAGE << std::endl;
		return 1;
	} else if (upx) {
		try {
			if (ExecuteShellCmd("upx --version 2>&1").substr(0, 3) != "upx") {
				throw std::runtime_error("upx command failed!");
			}
		} catch (std::exception const& e) {
			std::cout << "UPX compression disabled, failed to get UPX version!" << std::endl;
			upx = false;
		}
	}

	{
		const char* ENV_PATH = getenv("PATH");
		if (ENV_PATH == NULL) {
			std::cout << "$PATH environment variable is not set or empty!" << std::endl;
			return 1;
		}
		searchDir = StringSplit(ENV_PATH, ";"); // ; is path delimiter on windows
		if (searchDir.size() <= 0) {
			std::cout << "No search directories found, are you sure $PATH is set?" << std::endl;
			return 1;
		}
	}

	std::cout << "Copy Enabled: " << (copy ? "true" : "false") << std::endl;
	std::cout << "UPX Compression Enabled: " << (upx ? "true" : "false") << std::endl;
	std::cout << "Target Executable: " << executable << std::endl;
	std::cout << "Search directories: " << std::endl;
	for (auto path : searchDir) {
		std::cout << "  " << path << '\n';
	}
	std::cout << std::endl;

	Vector<String> dllsRequired;
	{
		Vector<String> __foundDLLs; // because GatherRequiredDLLs is a recursive function it passes the found dlls as reference to avoid duplicate entries but initially calling the function with empty vector works
		dllsRequired = GatherRequiredDLLs(executable, __foundDLLs);
	}

	std::cout << "Found DLLs: \n";
	for (String& dll : dllsRequired) {
		std::cout << " - " << dll << '\n';
		if (copy) {
			ExecuteShellCmd(("cp -f " + dll + " " + fs::path(executable).parent_path().string()).c_str());
		}
	}
	std::cout << std::endl;

	return 0;
}
