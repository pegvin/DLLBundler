# DLLB - DLL Bundler
Recursively Copy DLLs Required By A .exe or a .dll file.

---
## Features

- No External Runtime Required (Like in Python or Js Scripts)
- Recursively Find Dependencies (Finds The Dependencies Of Dependencies Of Dependencies...)
- Optionally Compress All The Dependencies Using UPX

---
## Requirements

- Objdump (Provider [Binutils](https://packages.msys2.org/package/binutils))
- UPX (Required Only If You Use Compression) (Provider [UPX](https://packages.msys2.org/package/upx))

---
## Usage
```
Usage: dllb [-h] [--copy] [--upx] executable

positional arguments:
  executable  EXE or DLL file to bundle dependencies for

options:
  -h, --help  Show this help message and exit
  --copy      Copy the DLLs next to the executable
  --upx       Run UPX on all the DLLs and EXE (requires --copy).
```

---
## Notes

- dllb uses directories listed in `$PATH` env variable to search for the required DLLs.
- overriding `$PATH` variable is useful if you want to add more search paths.

---
## Building From Source

**Requirements:**
- [Git](https://packages.msys2.org/package/git)
- [GNU Make](https://packages.msys2.org/package/make)
- [g++](https://packages.msys2.org/package/gcc) or [clang++](https://packages.msys2.org/package/clang)

1. Clone The Repo: `git clone https://github.com/pegvin/DLLBundler`
2. CD into the cloned repo: `cd DLLBundler/`
3. Build: `make all DEBUG=false` (`DEBUG` is set to true by default)

---
## License
DLLB is licensed under [BSD 3-Clause "New" or "Revised" License](./LICENSE).

---
# Thanks
