# runtime-detour

Replace a function **at runtime** by overwriting its entry with a jump to a replacement, with no recompile and no edit to its source. A C and x86-64/arm64 assembly demo of the "detour" / inline-hook technique behind Detours, funchook, and frida-gum.

## Prerequisites

- A C compiler / toolchain (Linux, x86-64 or arm64)

## Build & run

```sh
./build.sh && ./detour
```
