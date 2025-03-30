#include "ficheros.h"

  

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, RED "Sintaxis: leer <nombre_dispositivo> <ninodo>\n");
        return FALLO;
    }

    
    int ninodo = atoi(argv[2]);

    // Montar el dispositivo
    if (bmount(argv[1]) == FALLO) {
        fprintf(stderr, RED"Error al montar el dispositivo\n");
        return FALLO;
    }

    struct STAT stat;
    if (mi_stat_f(ninodo, &stat) == FALLO) {
        fprintf(stderr, RED "Error al obtener información del inodo\n");
        bumount();
        return FALLO;
    }

    // Buffer para la lectura
    char buffer_texto[BLOCKSIZE];// Tamaño del buffer de lectura para leer bloque a bloque 
    int offset = 0;
    int bytesLeidos = 0;
    int totBytesLeidos = 0;

    // Leer el contenido del inodo en bloques
    do {
        memset(buffer_texto, 0, BLOCKSIZE);  // Limpiar el buffer antes de cada lectura
        bytesLeidos = mi_read_f(ninodo, buffer_texto, offset, BLOCKSIZE);
    
        if (bytesLeidos == FALLO) {
            fprintf(stderr, "Error al leer del inodo\n");
            bumount();
            return EXIT_FAILURE;
        }

        // Escribir lo leído a la salida estándar
        write(1, buffer_texto, bytesLeidos);    
        totBytesLeidos += bytesLeidos;
        offset += BLOCKSIZE;
    } while (bytesLeidos > 0);

    // Mostrar estadísticas por stderr para que no afecte a la redirección
    char mensaje[128];
    sprintf(mensaje, "total_leidos %d\ntamEnBytesLog %d\n", totBytesLeidos, stat.tamEnBytesLog);
    write(2, mensaje, strlen(mensaje));

    // Desmontar el dispositivo
    if (bumount() == FALLO) {
        fprintf(stderr, RED"Error al desmontar el dispositivo\n");
        return FALLO;
    }

    return FALLO;
}