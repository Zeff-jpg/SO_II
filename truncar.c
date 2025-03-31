#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ficheros.h"

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Sintaxis: truncar <nombre_dispositivo> <ninodo> <nbytes>\n");
        return -1;
    }
    
    // Montar dispositivo
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "Error al montar el dispositivo\n");
        return -1;
    }
    
    unsigned int ninodo = atoi(argv[2]);
    unsigned int nbytes = atoi(argv[3]);
    
    int resultado;
    if (nbytes == 0) {
        resultado = liberar_inodo(ninodo);
        printf("Inodo %d liberado\n", resultado);
    } else {
        resultado = mi_truncar_f(ninodo, nbytes);
        printf("Bloques liberados: %d\n", resultado);
        
        // Mostrar información del inodo
        struct STAT stat;
        if (mi_stat_f(ninodo, &stat) == -1) {
            fprintf(stderr, "Error al obtener stat del inodo\n");
            return -1;
        }
        
        printf("tamEnBytesLog: %u\n", stat.tamEnBytesLog);
        printf("numBloquesOcupados: %u\n", stat.numBloquesOcupados);
    }
    
    // Desmontar dispositivo
    if (bumount() == -1) {
        fprintf(stderr, "Error al desmontar el dispositivo\n");
        return -1;
    }
    
    return 0;
}