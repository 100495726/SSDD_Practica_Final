#include "usuarios.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

static Usuario* lista_usuarios = NULL;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Función auxiliar para buscar un usuario en la lista
static Usuario* buscar_usuario(const char* nombre) {
    Usuario* actual = lista_usuarios;
    while (actual != NULL) {
        if (strcmp(actual->nombre, nombre) == 0) {
            return actual;
        }
        actual = actual->siguiente;
    }
    return NULL;
}

int register_user(const char* nombre) {
  	// Validar la entrada
    if (strlen(nombre) == 0 || strlen(nombre) >= MAX_USER_LEN) return 2;

    // Verificar si el usuario ya existe
    pthread_mutex_lock(&mutex);
    Usuario* existente = buscar_usuario(nombre);
    if (existente) {
        pthread_mutex_unlock(&mutex);
        return 1;
    }

    // Crear un nuevo usuario
    Usuario* nuevo = malloc(sizeof(Usuario));
    if (!nuevo) {
        pthread_mutex_unlock(&mutex);
        return 2;
    }

    strncpy(nuevo->nombre, nombre, MAX_USER_LEN);
    nuevo->conectado = 0;
    nuevo->ip[0] = '\0';
    nuevo->puerto[0] = '\0';
    nuevo->siguiente = lista_usuarios;
    lista_usuarios = nuevo;

    pthread_mutex_unlock(&mutex);
    return 0;
}

int unregister_user(const char* nombre) {
  	// Validar la entrada
  	if (strlen(nombre) == 0 || strlen(nombre) >= MAX_USER_LEN) return 2;

    pthread_mutex_lock(&mutex);
    Usuario* actual = lista_usuarios;
    Usuario* anterior = NULL;

    // Buscar al usuario
    while (actual != NULL) {
        if (strcmp(actual->nombre, nombre) == 0) {
          	// Usuario encontrado
            if (anterior) {
                anterior->siguiente = actual->siguiente;
            } else {
                lista_usuarios = actual->siguiente;
            }
            free(actual);
            pthread_mutex_unlock(&mutex);
            return 0;
        }
        anterior = actual;
        actual = actual->siguiente;
    }

    // Si el bucle termina, el usuario no ha sido econtrado
    pthread_mutex_unlock(&mutex);
    return 1;
}

int connect_user(const char* nombre, const char* ip, const char* puerto) {
  	// Validar la entrada
  	if (strlen(nombre) == 0 || strlen(nombre) >= MAX_USER_LEN) return 2;

    pthread_mutex_lock(&mutex);
    Usuario* usuario = buscar_usuario(nombre);

    // Verificar que el usuario está registrado en el sistema
    if (!usuario) {
        pthread_mutex_unlock(&mutex);
        return 1;
    }

    // Verificar que el usuario no esté ya conectado
    if (usuario->conectado) {
        pthread_mutex_unlock(&mutex);
        return 2;
    }

    // Verificar que la IP y el puerto sean válidos
    if (!ip || !puerto || strlen(ip) == 0 || strlen(puerto) == 0) {
    	pthread_mutex_unlock(&mutex);
    	return 3;
	}

    // Conectar al usuario al sistema
    usuario->conectado = 1;
    strncpy(usuario->ip, ip, sizeof(usuario->ip) - 1);
	usuario->ip[sizeof(usuario->ip) - 1] = '\0';
	strncpy(usuario->puerto, puerto, sizeof(usuario->puerto) - 1);
	usuario->puerto[sizeof(usuario->puerto) - 1] = '\0';

    pthread_mutex_unlock(&mutex);
    return 0;
}

int list_users(char* buffer) {
    pthread_mutex_lock(&mutex);
    Usuario* actual = lista_usuarios;
    int offset = 0;
    int count = 0;

    // Contar usuarios conectados
    while (actual != NULL) {
        if (actual->conectado) count++;
        actual = actual->siguiente;
    }

    // Escribir el número de usuarios
    offset += snprintf(buffer, MAX_USERS_LIST, "%d", count);

    // Escribir datos
    actual = lista_usuarios;
    while (actual != NULL) {
        if (actual->conectado) {
            int written = snprintf(buffer + offset, MAX_USERS_LIST - offset, "\n%s  %s  %s",
                                actual->nombre, actual->ip, actual->puerto);

            if (written < 0) {
                pthread_mutex_unlock(&mutex);
                return 2;
            }
            offset += written;
        }
        actual = actual->siguiente;
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}

int disconnect_user(const char* nombre) {
  	// Validar la entrada
  	if (strlen(nombre) == 0 || strlen(nombre) >= MAX_USER_LEN) return 2;

    pthread_mutex_lock(&mutex);
    Usuario* usuario = buscar_usuario(nombre);

    // Verificar que el usuario está registrado en el sistema
    if (!usuario) {
        pthread_mutex_unlock(&mutex);
        return 1;
    }

    // Verificar que el usuario está conectado al sistema
    if (!usuario->conectado) {
        pthread_mutex_unlock(&mutex);
        return 2;
    }

    // Desconectar al usuario del sistema
    usuario->conectado = 0;
    pthread_mutex_unlock(&mutex);
    return 0;
}