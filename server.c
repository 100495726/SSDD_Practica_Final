#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "usuarios.h"

#define MAX_BUFFER 1024

// Estructura para pasarle el socket y el mensaje al thread
typedef struct {
    int socket;
    char pet[MAX_BUFFER];
    char client_ip[INET_ADDRSTRLEN];
} ThreadData;

// Función que maneja cada solicitud en un hilo
void *tratar_peticion(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int sc = data->socket;
    char *pet = data->pet;
    char res[MAX_BUFFER] = {0};

    char *operacion = strtok(pet, " ");
    if (!operacion) {
        strcpy(res, "1 Invalid operation.");
        goto terminar;
    }

    if (strcmp(operacion, "REGISTER") == 0) {
        char *user = strtok(NULL, " ");
        if (!user) {
            strcpy(res, "ERROR");
            goto terminar;
        }

        int result = register_user(user);
        switch (result) {
          case 0:
            strcpy(res, "0 REGISTER OK");
            break;
          case 1:
			strcpy(res, "1 USERNAME IN USE");
            break;
          case 2:
            strcpy(res, "2 REGISTER FAIL");
            break;
          default:
            goto terminar;
        }
    }
    else if (strcmp(operacion, "UNREGISTER") == 0) {
        char *user = strtok(NULL, " ");
        if (!user) {
            strcpy(res, "ERROR");
            goto terminar;
        }

        int result = unregister_user(user);
        switch (result) {
          case 0:
            strcpy(res, "0 UNREGISTER OK");
            break;
          case 1:
			strcpy(res, "1 USER DOES NOT EXIST");
            break;
          case 2:
            strcpy(res, "2 UNREGISTER FAIL");
            break;
          default:
            goto terminar;
        }
    }
    else if (strcmp(operacion, "CONNECT") == 0) {
        char *user = strtok(NULL, " ");

        // Obtener IP y puerto del cliente conectado
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);
        getpeername(sc, (struct sockaddr*)&addr, &addr_len);

        char ip_str[INET_ADDRSTRLEN];
        char port_str[6];

        inet_ntop(AF_INET, &addr.sin_addr, ip_str, sizeof(ip_str));
        snprintf(port_str, sizeof(port_str), "%d", ntohs(addr.sin_port));

        int result = connect_user(user, ip_str, port_str);

        switch (result) {
          case 0:
            strcpy(res, "0 CONNECT OK");
            break;
          case 1:
			strcpy(res, "1 CONNECT FAIL, USER DOES NOT EXIST");
            break;
          case 2:
            strcpy(res, "2 USER ALREADY CONNECTED");
            break;
          case 3:
            strcpy(res, "3 UNREGISTER FAIL");
            break;
          default:
            goto terminar;
        }
    }
    else if (strcmp(operacion, "LIST_USERS") == 0) {
        char lista[MAX_BUFFER] = {0};
        char *user = strtok(NULL, " ");

        if (!user) {
            strcpy(res, "ERROR");
            goto terminar;
        }

        int result = list_users(lista, user);
        switch (result) {
          case 0:
            snprintf(res, MAX_BUFFER, "0 LIST_USERS OK %s", lista);
            break;
          case 1:
			      strcpy(res, "1 LIST_USERS FAIL, USER DOES NOT EXIST");
            break;
          case 2:
            strcpy(res, "2 LIST_USERS FAIL, USER NOT CONNECTED");
            break;
          case 3:
            strcpy(res, "3 LIST_USERS FAIL");
            break;
          default:
            goto terminar;
        }
    }
    else if (strcmp(operacion, "DISCONNECT") == 0) {
        char *user = strtok(NULL, " ");
        if (!user) {
            strcpy(res, "ERROR");
            goto terminar;
        }

        int result = disconnect_user(user);
        switch (result) {
          case 0:
            strcpy(res, "0 DISCONNECT OK");
            break;
          case 1:
			strcpy(res, "1 DISCONNECT FAIL, USER DOES NOT EXIST");
            break;
          case 2:
            strcpy(res, "2 DISCONNECT FAIL, USER NOT CONNECTED");
            break;
          case 3:
            strcpy(res, "3 DISCONNECT FAIL");
            break;
          default:
            goto terminar;
        }
    }
    else if (strcmp(operacion, "PUBLISH") == 0) {
        char *user = strtok(NULL, " ");
        char *filename = strtok(NULL, " ");
        char *description = strtok(NULL, " ");

        if (!user) {
            strcpy(res, "ERROR");
            goto terminar;
        }

        if (!filename) {
            strcpy(res, "ERROR");
            goto terminar;
        }
        if (!description) {
            strcpy(res, "ERROR");
            goto terminar;
        }

        int result = publish_file(user, filename, description);
        switch (result) {
          case 0:
            strcpy(res, "0 PUBLISH OK");
            break;
          case 1:
			strcpy(res, "1 PUBLISH FAIL, USER DOES NOT EXIST");
            break;
          case 2:
            strcpy(res, "2 PUBLISH FAIL, USER NOT CONNECTED");
            break;
          case 3:
            strcpy(res, "3 PUBLISH FAIL, CONTENT ALREADY PUBLISHED");
            break;
          case 4:
            strcpy(res, "4 PUBLISH FAIL");
            break;
          default:
            goto terminar;
        }
    }
    else if (strcmp(operacion, "DELETE") == 0) {
        char *user = strtok(NULL, " ");
        char *filename = strtok(NULL, " ");

        if (!user) {
            strcpy(res, "ERROR");
            goto terminar;
        }

        if (!filename) {
            strcpy(res, "ERROR");
            goto terminar;
        }

        int result = delete_file(user, filename);
        switch (result) {
          case 0:
            strcpy(res, "0 DELETE OK");
            break;
          case 1:
			      strcpy(res, "1 DELETE FAIL, USER DOES NOT EXIST");
            break;
          case 2:
            strcpy(res, "2 DELETE FAIL, USER NOT CONNECTED");
            break;
          case 3:
            strcpy(res, "3 DELETE FAIL, CONTENT NOT PUBLISHED");
            break;
          case 4:
            strcpy(res, "4 PUBLISH FAIL");
            break;
          default:
            goto terminar;
        }
    }
    else if (strcmp(operacion, "LIST_CONTENT") == 0) {
        char lista[MAX_BUFFER] = {0};

        char *user_activo = strtok(NULL, " ");
        char *user_buscado = strtok(NULL, " ");

        if (!user_activo) {
            strcpy(res, "ERROR");
            goto terminar;
        }

        if (!user_buscado) {
            strcpy(res, "ERROR");
            goto terminar;
        }
        
        int result = list_content(lista, user_activo, user_buscado);

        switch (result) {
          case 0:
            snprintf(res, MAX_BUFFER, "0 LIST_CONTENT OK %s", lista);
            break;
          case 1:
			      strcpy(res, "1 LIST_CONTENT FAIL, USER DOES NOT EXIST");
            break;
          case 2:
            strcpy(res, "2 LIST_CONTENT FAIL, USER NOT CONNECTED");
            break;
          case 3:
            strcpy(res, "3 LIST_CONTENT FAIL, REMOTE USER DOES NOT EXIST");
            break;
          case 4:
            strcpy(res, "4 LIST_CONTENT FAIL");
            break;
          default:
            goto terminar;
        }
    }
    else {
        strcpy(res, "ERROR");
    }

terminar:
    if (send(sc, res, strlen(res), 0) < 0) {
        perror("Error al enviar respuesta");
    }
    close(sc);
    free(data);
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    // Comprobar que se pasa el puerto por la línea de mandatos
    if (argc != 2) {
      perror("Uso del servidor: ./servidor <PUERTO>");
      return -1;
    }

    struct sockaddr_in server_addr, client_addr;
    socklen_t size;
    int sd, sc;
    int val;

    // Crear el socket
    if ((sd = socket(AF_INET, SOCK_STREAM, 0))<0){
        perror("SERVER: Error en el socket");
        return (0);
    }

    val = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(sd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en el bind");
        close(sd);
        return -1;
    }

    if (listen(sd, SOMAXCONN) < 0) {
        perror("Error en el listen");
        close(sd);
        return -1;
    }

    size = sizeof(client_addr);

    while (1) {
        // Aceptar la conexión
        printf("init server %s\n", argv[1]);
        sc = accept(sd, (struct sockaddr *)&client_addr, &size);
        if (sc < 0) {
            perror("Error al aceptar la conexion");
            continue;
        }

        // Obtener la IP del cliente
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

        printf("Conexión establecida con el cliente\n");

        // Recibir la petición
        char *pet = malloc(MAX_BUFFER);
        int bytes = recv(sc, pet, MAX_BUFFER - 1, 0);
        if (bytes <= 0) {
            perror("Error al leer la peticion");
            close(sc);
            free(pet);
            continue;
        }
        pet[bytes] = '\0';

        // Preparar los datos del thread
        ThreadData *data = malloc(sizeof(ThreadData));
        data->socket = sc;
        strcpy(data->pet, pet);
        strcpy(data->client_ip, client_ip);
        free(pet);

        // Crear un nuevo hilo para manejar la solicitud
        pthread_t thread;
        int err = pthread_create(&thread, NULL, tratar_peticion, data);
        if (err != 0) {
            fprintf(stderr, "Error al crear hilo: %s\n", strerror(err));
            free(data);
            close(sc);
        } else {
            pthread_detach(thread);
        }
    }

    // Cerrar el socket del servidor (esto nunca se ejecuta porque el while es infinito)
    close(sd);
    return 0;
}
