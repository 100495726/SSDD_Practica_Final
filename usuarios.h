#ifndef USUARIOS_H
#define USUARIOS_H

#define MAX_USER_LEN 256
#define MAX_FILE_LEN 256
#define MAX_USERS_LIST 1024

typedef struct Usuario {
    char nombre[MAX_USER_LEN];
    int conectado;
    char ip[16];
    char puerto[6];
    struct Usuario* siguiente;
} Usuario;

int register_user(const char* nombre);
int unregister_user(const char* nombre);
int connect_user(const char* nombre, const char* ip, const char* puerto);
int list_users(char* buffer);
int disconnect_user(const char* nombre);

#endif