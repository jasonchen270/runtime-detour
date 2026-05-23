# runtime-detour

Replace a function **at runtime** by overwriting its entry with a jump to a
replacement, no recompile, no edit to its source. The "detour" / inline-hook
technique behind Detours, funchook, and frida-gum.

```
before: answer() = 1
after:  answer() = 42
```

## How it works

A compiled function has one entry address that every call funnels through. Make
its code page writable and overwrite the first instruction with a jump to your
replacement:

```
x86-64 : 5 bytes   E9 <rel32>     jmp <replacement>      (relative, +-2GB)
arm64  : 4 bytes   B  <imm26>     branch <replacement>   (relative, +-128MB)
                                  + __builtin___clear_cache (icache isn't coherent)
```

This is **pure replace**: the original isn't preserved, every call goes to the
replacement. (Calling the original too would need a trampoline, copy out the
overwritten instructions, relocate their RIP-relative refs, and jump back, out of
scope here.)

The patch uses a single `mprotect` to RWX rather than RW-then-RX, because the
patcher can share a code page with its target and dropping EXEC even briefly
would fault the patcher mid-execution.

## Build & run

Linux, x86-64 or arm64:

```sh
./build.sh && ./detour
```
