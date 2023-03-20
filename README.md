# DLLBundler
Recursively Copy DLLs Required By A .exe or a .dll file.

---
# Usage
```
Usage: dllbundler [-h] [--copy] [--upx] [--search-dir SEARCH_DIR] executable

positional arguments:
  executable               EXE or DLL file to bundle dependencies for

options:
  -h, --help               Show this help message and exit
  --copy                   Copy the DLLs next to the executable
  --upx                    Run UPX on all the DLLs and EXE (requires --copy).
```

---
# Thanks
