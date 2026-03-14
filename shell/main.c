

extern void _test(void);

int main(void) {

    while (1) {
        for (int i = 0; i < 1000000000; i++)
            ;

        _test();
    }

    return 0;
}
