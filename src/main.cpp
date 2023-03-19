#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include "utils.h"

namespace fs = std::filesystem;

#define HELP_MESSAGE "Usage: dllbundler [-h] [--copy] [--upx] [--search-dir SEARCH_DIR] executable\n\n" \
"positional arguments:\n" \
"  executable               EXE or DLL file to bundle dependencies for\n\n" \
"options:\n" \
"  -h, --help               Show this help message and exit\n" \
"  --copy                   Copy the DLLs next to the executable\n" \
"  --upx                    Run UPX on all the DLLs and EXE (requires --copy)."

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
		String dllPathLower = (fs::path(directory) / fs::path(StringLowerCaseCopy(dllName))).string();
		if (doesFileExist(dllPath)) {
			std::cout << "Found: " << dllPath << std::endl;
			return dllPath;
		} else if (dllPath != dllPathLower && doesFileExist(dllPathLower)) {
			std::cout << "Found: " << dllPathLower << std::endl;
			return dllPathLower;
		}
	}
	std::cout << "Can't find \"" << dllName << '"' << std::endl;
	return "";
}

Vector<String> GatherRequiredDLLs(String& binaryPath, Vector<String>& foundDLLs) {
	String output = ExecuteShellCmd(("objdump -p " + binaryPath + " | grep '\tDLL Name:' 2>&1").c_str());
	auto requiredDLLs = StringSplit(output, "\tDLL Name: ");

	// filter out if it exists in blacklist or foundDLLs
	requiredDLLs.erase(std::remove_if(std::begin(requiredDLLs), std::end(requiredDLLs), [&](String& dll) {
		trim(dll);
		if (dll.empty()) return true;
		auto lowerDLL = StringLowerCaseCopy(dll);
		for (auto blacklistDLL : blacklist) {
			if (lowerDLL == blacklistDLL) return true;
		}
		for (auto foundDLL : foundDLLs) {
			if (dll == foundDLL) return true;
		}

		String dllPath = FindDLLPath(dll, searchDir);
		if (!dllPath.empty()) foundDLLs.emplace_back(dllPath);

		auto subDLLs = GatherRequiredDLLs(dll, requiredDLLs);
		if (subDLLs.size() > 0) {
			requiredDLLs.insert(
				requiredDLLs.end(),
				std::make_move_iterator(subDLLs.begin()),
				std::make_move_iterator(subDLLs.end())
			);
		}

		return false;
	}), std::end(requiredDLLs));

	return requiredDLLs;
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

	std::cout << "DLLs Required: \n";
	for (String& dll : dllsRequired) {
		std::cout << "  " << dll << '\n';
	}
	std::cout << std::endl;

	return 0;
}

