/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include "ficheros.h"
#include <string.h>
#include <stdio.h>

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes) {
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) == FALLO) {
        fprintf(stderr,RED "Error al leer el inodo\n");
        fprintf(stderr,RESET);
        return FALLO;
    }

    if ((inodo.permisos & 2) != 2) {
        fprintf(stderr,RED "No hay permisos de escritura\n");
        fprintf(stderr,RESET);
        return FALLO;
    }

    unsigned int primerBL = offset / BLOCKSIZE;
    unsigned int ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;
    unsigned int desp1 = offset % BLOCKSIZE;
    unsigned int desp2 = (offset + nbytes - 1) % BLOCKSIZE;
    unsigned int bytes_escritos = 0;
    char buf_bloque[BLOCKSIZE];

    if(primerBL==ultimoBL){
            // Escribir en el primer bloque lógico
        int nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);
        if (nbfisico == FALLO) return FALLO;
        if (bread(nbfisico, buf_bloque) == FALLO) return FALLO;
        memcpy(buf_bloque + desp1, buf_original, nbytes);
        if (bwrite(nbfisico, buf_bloque) == FALLO) return FALLO;
        bytes_escritos = nbytes;

        // Actualizar metadatos del inodo
        if (offset + nbytes > inodo.tamEnBytesLog) {
            inodo.tamEnBytesLog = offset + nbytes;
            
        }
        inodo.mtime = time(NULL);
        inodo.ctime = time(NULL);
        if (escribir_inodo(ninodo, &inodo) == FALLO) return FALLO;
        return bytes_escritos;
    }else{
        // Escribir en el primer bloque lógico
        int nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);
        if (nbfisico == FALLO) return FALLO;

        if (bread(nbfisico, buf_bloque) == FALLO) return FALLO;
        memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);
        if (bwrite(nbfisico, buf_bloque) == FALLO) return FALLO;
        bytes_escritos += BLOCKSIZE - desp1;
    
        // Escribir en los bloques intermedios
        for (unsigned int bl = primerBL + 1; bl < ultimoBL; bl++) {
            nbfisico = traducir_bloque_inodo(ninodo, bl, 1);
            if (nbfisico == FALLO) return FALLO;
            if (bwrite(nbfisico, buf_original + bytes_escritos) == FALLO) return FALLO;
            bytes_escritos += BLOCKSIZE;
        }

        // Escribir en el último bloque lógico
        if (primerBL != ultimoBL) {
            nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 1);
            if (nbfisico == FALLO) return FALLO;
            if (bread(nbfisico, buf_bloque) == FALLO) return FALLO;
            memcpy(buf_bloque, buf_original + bytes_escritos, desp2 + 1);
            if (bwrite(nbfisico, buf_bloque) == FALLO) return FALLO;
            bytes_escritos += desp2 + 1;
        }

        // Actualizar metadatos del inodo
      
        if (offset + nbytes > inodo.tamEnBytesLog) {
            inodo.tamEnBytesLog = offset + nbytes;
            
        }
        inodo.mtime = time(NULL);
        inodo.ctime = time(NULL);
        if (escribir_inodo(ninodo, &inodo) == FALLO) return FALLO;

        return bytes_escritos;
    }
}

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes) {
    struct inodo inodo;
    unsigned int nbloque, bloque_fisico;
    unsigned char buf_bloque[BLOCKSIZE];
    unsigned int bytes_leidos = 0;
    unsigned int desp_inicio;

    // 1. Leer el inodo desde el dispositivo
    if (leer_inodo(ninodo, &inodo) == FALLO) {
        fprintf(stderr, "Error al leer inodo %d\n", ninodo);
        return FALLO;
    }

    // 2. Verificar permisos de lectura
    if ((inodo.permisos & 4) != 4) {
        fprintf(stderr, "Permiso denegado de lectura en inodo %d\n", ninodo);
        return FALLO;
    }

    // 3. Si el offset está más allá del EOF, no hay nada que leer
    if (offset >= inodo.tamEnBytesLog) {
        return 0;
    }

    // 4. Ajustar nbytes si intentamos leer más allá del EOF
    if (offset + nbytes > inodo.tamEnBytesLog) {
        nbytes = inodo.tamEnBytesLog - offset;
    }

    // 5. Calcular bloque lógico inicial y desplazamiento dentro del bloque
    nbloque = offset / BLOCKSIZE;
    desp_inicio = offset % BLOCKSIZE;

    // 6. Bucle principal de lectura
    while (bytes_leidos < nbytes) {
        // 6.1 Traducir bloque lógico a físico (sin reservar, solo lectura)
        bloque_fisico = traducir_bloque_inodo(ninodo, nbloque, 0);
        if (bloque_fisico == FALLO) {
            break; // No hay más bloques asignados
        }

        // 6.2 Leer el bloque físico completo
        if (bread(bloque_fisico, buf_bloque) == FALLO) {
            fprintf(stderr, "Error al leer bloque físico %d\n", bloque_fisico);
            break;
        }

        // 6.3 Calcular cuántos bytes leer de este bloque
        unsigned int bytes_en_bloque = MIN(BLOCKSIZE - desp_inicio, nbytes - bytes_leidos);

        // 6.4 Copiar datos al buffer de salida
        memcpy((char *)buf_original + bytes_leidos, buf_bloque + desp_inicio, bytes_en_bloque);

        // 6.5 Actualizar contadores y punteros
        bytes_leidos += bytes_en_bloque;
        nbloque++;
        desp_inicio = 0; // Para los siguientes bloques, empezamos desde el principio
    }

    // 7. Actualizar atime del inodo (último acceso)
    leer_inodo(ninodo, &inodo); // Volver a leer por posibles cambios concurrentes
    inodo.atime = time(NULL);
    escribir_inodo(ninodo, &inodo);

    return bytes_leidos;
}

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat) {
    struct inodo inodo;
    
    // Leemos el inodo del sistema de archivos
    if (leer_inodo(ninodo, &inodo) == FALLO) {
        fprintf(stderr, "Error al leer el inodo\n");
        return FALLO;
    }
    
    // Copiamos la metainformación relevante al struct STAT
    p_stat->tipo = inodo.tipo;
    p_stat->permisos = inodo.permisos;
    p_stat->nlinks = inodo.nlinks;
    p_stat->tamEnBytesLog = inodo.tamEnBytesLog;
    p_stat->atime = inodo.atime;
    p_stat->mtime = inodo.mtime;
    p_stat->ctime = inodo.ctime;
    p_stat->numBloquesOcupados = inodo.numBloquesOcupados;
    
    return EXITO;
}

int mi_chmod_f(unsigned int ninodo, unsigned char permisos) {
    struct inodo inodo;

    // Leemos el inodo correspondiente
    if (leer_inodo(ninodo, &inodo) == FALLO) {
        fprintf(stderr, RED "Error al leer el inodo\n" RESET);
        return FALLO;
    }

    // Asignamos los nuevos permisos
    inodo.permisos = permisos;

    // Actualizamos el ctime porque hemos modificado metadatos
    inodo.ctime = time(NULL);

    // Guardamos el inodo actualizado
    if (escribir_inodo(ninodo, &inodo) == FALLO) {
        fprintf(stderr, RED "Error al escribir el inodo\n" RESET);
        return FALLO;
    }

    return EXITO;
}
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes) {
    struct inodo inodo;
    
    // Leer el inodo
    if (leer_inodo(ninodo, &inodo) == -1) {
        return -1;
    }
    
    // Comprobar permisos de escritura
    if ((inodo.permisos & 2) != 2) {
        fprintf(stderr, "Error: permisos denegados de escritura\n");
        return -1;
    }
    
    // No se puede truncar más allá del EOF
    if (nbytes >= inodo.tamEnBytesLog) {
        fprintf(stderr, "Error: no se puede truncar más allá del EOF\n");
        return -1;
    }
    
    // Calcular primer bloque lógico a liberar
    unsigned int primerBL;
    if (nbytes % BLOCKSIZE == 0) {
        primerBL = nbytes / BLOCKSIZE;
    } else {
        primerBL = nbytes / BLOCKSIZE + 1;
    }
    
    // Liberar bloques desde primerBL hasta el final
    int bloques_liberados = liberar_bloques_inodo(primerBL, &inodo);
    inodo.numBloquesOcupados -= bloques_liberados;
    
    // Actualizar metadatos
    inodo.tamEnBytesLog = nbytes;
    inodo.mtime = time(NULL);
    inodo.ctime = time(NULL);
    
    // Escribir el inodo actualizado
    if (escribir_inodo(ninodo, &inodo) == -1) {
        return -1;
    }
    
    return bloques_liberados;
}

