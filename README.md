# runtime-detour

Replace a function **at runtime** by overwriting its entry with a jump to a
replacement, no recompile, no edit to its source. The "detour" / inline-hook
technique behind Detours, funchook, and frida-gum.

```
before: answer() = 1
after:  answer() = 42
```

## Build & run

Linux, x86-64 or arm64:

```sh
./build.sh && ./detour
```
