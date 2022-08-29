int main(void) {
    __asm__ __volatile__("mov $0, %rax; int $0x80");
    while (1);
    return 0;
}
