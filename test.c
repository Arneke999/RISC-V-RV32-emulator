static void write(const char *s, int n) {
    register int a0 asm("a0") = 1, a7 asm("a7") = 64;
    register const char *a1 asm("a1") = s;
    register int a2 asm("a2") = n;
    asm volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a7) : "memory");
}
int main(void) { write("hello from rv32\n", 16); return 42; }