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
        fprintf(stderr, "Error al leer el inodo\n");
        return FALLO;
    }

    if ((inodo.permisos & 2) != 2) {
        fprintf(stderr, "No hay permisos de escritura\n");
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
    if (leer_inodo(ninodo, &inodo) == FALLO) return FALLO;

    if ((inodo.permisos & 4) != 4) {
        fprintf(stderr, "No hay permisos de lectura\n");
        return FALLO;
    }

    if (offset >= inodo.tamEnBytesLog) return 0;

    if ((offset + nbytes) >= inodo.tamEnBytesLog) {
        nbytes = inodo.tamEnBytesLog - offset;
    }

    unsigned int primerBL = offset / BLOCKSIZE;
    unsigned int ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;
    unsigned int desp1 = offset % BLOCKSIZE;
    unsigned int desp2 = (offset + nbytes - 1) % BLOCKSIZE;

    unsigned char buf_bloque[BLOCKSIZE];
    int leidos = 0;
    unsigned int nbfisico;

    if (primerBL == ultimoBL) {
        nbfisico = traducir_bloque_inodo(ninodo, primerBL, 0);
        if (nbfisico != -1) {
            if (bread(nbfisico, buf_bloque) == FALLO) return FALLO;
            memcpy(buf_original, buf_bloque + desp1, nbytes);
        }
        leidos += nbytes;
    } else {
        //primer bloque
        nbfisico = traducir_bloque_inodo(ninodo, primerBL, 0);
        if (nbfisico != -1) {
            if (bread(nbfisico, buf_bloque) == FALLO) return FALLO;
            memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);
        }
        leidos += BLOCKSIZE - desp1;
        //bloques intermedios
        for (unsigned int bl = primerBL + 1; bl < ultimoBL; bl++) {
            nbfisico = traducir_bloque_inodo(ninodo, bl, 0);
            if (nbfisico != -1) {
                if (bread(nbfisico, buf_original + leidos) == FALLO) return FALLO;
            }
            leidos += BLOCKSIZE;
        }
        //ultimo bloque
        nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 0);
        if (nbfisico != -1) {
            if (bread(nbfisico, buf_bloque) == FALLO) return FALLO;
            memcpy(buf_original + leidos, buf_bloque, desp2 + 1);
        }
        leidos += desp2 + 1;
    }
    
    inodo.atime = time(NULL);
    if (escribir_inodo(ninodo, &inodo) == FALLO) return FALLO;
    return leidos;
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

