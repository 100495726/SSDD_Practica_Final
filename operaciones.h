#ifndef OPERACIONES_H
#define OPERACIONES_H

#define MAX_USER_LEN 256
#define MAX_FILE_LEN 256
#define MAX_USERS_LIST 1024
#define MAX_FILE_DESCRIPTION 256
#define MAX_FILES_LIST 1024

typedef struct Usuario {
    char nombre[MAX_USER_LEN];
    int conectado;
    char ip[16];
    char puerto[6];
    struct Usuario* siguiente;
} Usuario;

typedef struct Fichero {
    char nombre_user[MAX_USER_LEN];
    char nombre_file[MAX_FILE_LEN];
    char description[MAX_FILE_DESCRIPTION];
    struct Fichero* siguiente;
} Fichero;

// FUNCIONES DE USUARIO
int register_user(const char* nombre);
int unregister_user(const char* nombre);
int connect_user(const char* nombre, const char* ip, const char* puerto);
int list_users(char* buffer, const char* username);
int disconnect_user(const char* nombre);

// FUNCIONES DE FICHERO
int publish_file(const char* username, const char* filename, const char* description);
int delete_file(const char* username, const char* filename);
int list_content(char* buffer, const char* username_activo, const char* username_buscado);

#endif