# rm-toto

Faithful CLI replica of GNU Core Utilities `rm`. Removes directory entries via
`unlink(2)` and `rmdir(2)` — never file data directly. Recursive removal
(`-r`/`-R`) walks the tree in post-order using `opendir`/`readdir`/`lstat`;
symlinks are never dereferenced.

## Build

```sh
make
make debug
make test
make clean
make install   # optional — installs to /usr/local/bin
```

Binaries land under `build/`: `build/rm-toto` from `make`; `build/tests/test_core`
from `make test`.

### Linux

```sh
make clean && make
```

### Windows (MSYS2 UCRT64)

```sh
pacman -S make mingw-w64-ucrt-x86_64-gcc
make clean && make
```

The native Windows `.exe` runs in UCRT64, Git Bash, cmd, and
PowerShell. Removing an **open** file may fail on Windows
(`ERROR_SHARING_VIOLATION`), unlike Linux.

## Usage

```sh
rm-toto [OPTION]... [FILE]...
```

Flags:

- `-f`, `--force` — ignore nonexistent files; never prompt
- `-i`, `--interactive` / `--interactive=always` — prompt before every removal
- `-I`, `--interactive=once` — one prompt when removing 3+ files or recursively
- `--interactive=never` — never prompt (without `-f`'s missing-file leniency)
- `-r`, `-R`, `--recursive` — remove directories and their contents
- `-d`, `--dir` — remove empty directories
- `-v`, `--verbose` — print each removed path
- `--preserve-root[=all]` — refuse `/` (default on); `=all` also refuses mount points
- `--no-preserve-root` — allow recursive removal of `/`
- `--one-file-system` — skip subdirectories on other devices when recursing
- `--` — end of options (required for names beginning with `-`, e.g. `rm-toto -- -foo`)

`--help` / `--h` and `--version` / `--v` may appear anywhere in `argv`; help wins
if both are present.

Conflict rule: among `-f`, `-i`, and `-I`, the **last** one given wins.

`--preserve-root` is **on by default**. `rm-toto -r /` is refused; use
`--no-preserve-root` to override (dangerous).

## Exit codes

| Code | Meaning |
|------|---------|
| `0` | Help, version, or all requested removals succeeded (including `-f`-suppressed missing files) |
| `1` | Any removal failed, missing operand without `-f`, or `--preserve-root` refusal |

Stderr diagnostics use the form `rm-toto: <context>: <reason>`.

## Layout

```
LICENSE.txt
c_version.txt
Makefile
README.md
include/rm_toto.h
include/rm_toto_*.h
src/main.c
src/rm_toto_{emit,walk,prompt,cli}.c
tests/test_runner.c
tests/test_join_path_*.c / test_is_root_path.c   (sources)
build/rm-toto                                      (build output; not tracked)
build/tests/test_core                              (build output; not tracked)
build/.gitkeep
build/tests/.gitkeep
```
