
struct Tupla {
    opaque username[256];
    opaque operation[20];
    opaque filename[256];
    opaque time[20];
};

program PRINT_REQUEST_PROG {
    version PRINT_REQUEST_VERS {
        int PRINT_REQUEST(Tupla) = 1;
    } = 1;
} = 0x20000001;