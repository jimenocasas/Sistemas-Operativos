struct Coord_rpc {
    int x;
    int y;
};

struct Entrada {
    int key;
    string value1<256>;
    int N_value2;
    double V_value2<32>;
    struct Coord_rpc value3;
};

struct RespuestaGet {
    int resultado;
    string value1<256>;
    int N_value2;
    double V_value2<32>;
    struct Coord_rpc value3;
};

struct SoloClave {
    int key;
};

program CLAVES_PROG {
    version CLAVES_VERS {
        int DESTROY(void) = 1;
        int SET_VALUE(Entrada) = 2;
        RespuestaGet GET_VALUE(SoloClave) = 3;
        int MODIFY_VALUE(Entrada) = 4;
        int DELETE_KEY(SoloClave) = 5;
        int EXIST(SoloClave) = 6;
    } = 1;
} = 0x20000001;
