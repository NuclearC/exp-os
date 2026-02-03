
extern void _v_print(const char* str, int row, int col);
extern void _io_read_sector(char* buf, int sector);

const char* long_text = "asdfausdhfasdufhuiashdfuhuasdfhuihuzhxcivuizxchvuihziuxcvhuizhxcuivhzuixchvuihzxcuivhuihdfuivhusdahfvgasdyfugyuasgdfyugyuqwegrfuygqwyufgyuqyudfqa\0";

int KeMain(void) {
    _v_print("Hello World", 0, 0);
    
    char buf[512];
    _io_read_sector(buf, 2);

    _v_print(buf + 4, 1, 0);
    _v_print(long_text, 2, 4);

    while (1) ;
    return 0;
}



