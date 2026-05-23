// detour.cpp -- replace a function at runtime by overwriting its entry with a
// jump to a replacement. PURE REPLACE: the original is not preserved (we never
// call it); the old behavior is visible in the source, so nothing is lost.
//
// Arch-aware:
//   x86-64 : 5-byte  E9 <rel32>            relative near jump
//   arm64  : 4-byte  B <imm26>             relative branch (+-128MB) + icache flush
//
//   build & run:  ./build.sh && ./detour      (Linux only -- see README)
// Expected:
//   arch: ...
//   before: answer() = 1
//   after:  answer() = 42
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>

__attribute__((noinline)) int answer()      { __asm__ volatile(""); return 1; }
__attribute__((noinline)) int replacement() { __asm__ volatile(""); return 42; }

static bool detour(void* from, void* to) {
    uint8_t* p = (uint8_t*)from;

#if defined(__x86_64__)
    const size_t N = 5;
    uint8_t code[N];
    int32_t rel = (int32_t)((uint8_t*)to - (p + 5));  // E9 is rel to end of insn
    code[0] = 0xE9;
    std::memcpy(code + 1, &rel, 4);
#elif defined(__aarch64__)
    const size_t N = 4;
    uint8_t code[N];
    int64_t off = (uint8_t*)to - p;                   // B is rel to the instruction
    uint32_t insn = 0x14000000u | ((uint32_t)(off >> 2) & 0x03FFFFFFu);
    std::memcpy(code, &insn, 4);
#else
#  error "unsupported arch"
#endif

    long ps = sysconf(_SC_PAGESIZE);
    uintptr_t page = (uintptr_t)p & ~(uintptr_t)(ps - 1);
    size_t span = (size_t)(p - (uint8_t*)page) + N;

    // One RWX call, NOT two-step RW->RX: the patcher (this function) may share a
    // page with the target, and dropping EXEC even briefly would fault us mid-
    // execution. Linux allows RWX; keep EXEC the whole time.
    if (mprotect((void*)page, span, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
        perror("mprotect");
        return false;
    }
    std::memcpy(p, code, N);
    __builtin___clear_cache((char*)p, (char*)p + N);  // required on arm64; no-op on x86
    return true;
}

int main() {
    int (*volatile fp)() = answer;  // volatile -> a real indirect call, not folded
#if defined(__x86_64__)
    std::printf("arch: x86-64\n");
#elif defined(__aarch64__)
    std::printf("arch: arm64\n");
#endif
    std::printf("before: answer() = %d\n", fp());
    std::fflush(stdout);
    if (detour((void*)answer, (void*)replacement)) {
        std::printf("after:  answer() = %d\n", fp());
        std::fflush(stdout);
    }
    return 0;
}
