/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include "fichero_basico.h"

int bitSobrantes(int n); // Funcion que devuelve valor decimal de los bits sobrantes//

/*Función que devuelve en numero de bloques necesarios para el Mapa de Bits */
int tamMB(unsigned int nbloques)
{

    int tamMB = (nbloques / 8) / BLOCKSIZE;

    if (((nbloques / 8) % BLOCKSIZE) == 0)
    {
        return tamMB;
    }
    else
    {
        return tamMB + 1;
    }
}

/*Función que calcula el tamaño de bloques del array de inodos*/
// paso de parametros desde mi_mkfs ninodos=nbloques/4//
int tamAI(unsigned int ninodos)
{

    int tamAI = (ninodos * INODOSIZE) / BLOCKSIZE;
    if (((ninodos * INODOSIZE) % BLOCKSIZE) == 0)
    {
        return tamAI;
    }
    else
    {
        return tamAI + 1;
    }
}

/*Función para inicializar el super bloque*/
int initSB(unsigned int nbloques, unsigned int ninodos)
{

    struct superbloque SB;

    // Posiciones del Mapa de bits //
    SB.posPrimerBloqueMB = 0 + 1; // posSB = 0 posMB = 1

    SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) - 1;

    // Posiciones del array de inodos//
    SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;

    SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos) - 1;

    // Posiciones del bloque de datos//
    SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;

    SB.posUltimoBloqueDatos = nbloques - 1;

    // posiciones de inodos//
    SB.posInodoRaiz = 0;

    SB.posPrimerInodoLibre = 0;

    // Cantidad de bloques e inodos libres  inicialmente//

    SB.cantBloquesLibres = nbloques;

    SB.cantInodosLibres = ninodos;

    // Cantidad totales de inodos y bloques //

    SB.totBloques = nbloques;

    SB.totInodos = ninodos;

    // Escribimos el superbloque en el disco virtual//
    // se puede escribir de esta manera ya que el super bloque ocupa 1 bloque//
    if (bwrite(posSB, &SB) == FALLO)
    {
        perror(RED "Error al escribir el SB");
        printf(RESET);
        return FALLO;
    };

    return 0;
}

int initMB()
{struct superbloque SB;
    if (bread(posSB, &SB) == -1) {
        fprintf(stderr, "Error leyendo el superbloque\n");
        return -1;
    }

    unsigned char bufferMB[BLOCKSIZE];
    memset(bufferMB, 0, BLOCKSIZE); // Inicializar todo el buffer a 0

    unsigned int bloquesOcupados = SB.posPrimerBloqueDatos; // Metadatos ocupan hasta posPrimerBloqueDatos
    unsigned int bytesLlenos = bloquesOcupados / 8;
    unsigned int bitResto = bloquesOcupados % 8;

    // Llenar completamente los bytes necesarios a 1
    for (unsigned int i = 0; i < bytesLlenos; i++) {
        bufferMB[i] = 255;
    }

    // Si hay bits sobrantes, configurarlos correctamente
    if (bitResto > 0) {
        bufferMB[bytesLlenos] = bitSobrantes(bitResto);
    }

    // Escribir en los bloques correspondientes del MB
    unsigned int numBloquesMB = tamMB(SB.totBloques);
    for (unsigned int i = SB.posPrimerBloqueMB; i < SB.posPrimerBloqueMB + numBloquesMB; i++) {
        if (bwrite(i, bufferMB) == -1) {
            fprintf(stderr, "Error escribiendo bloque del MB %d\n", i);
            return -1;
        }
    }

    // Actualizar superbloque con la cantidad de bloques libres
    SB.cantBloquesLibres -= bloquesOcupados;
    if (bwrite(posSB, &SB) == -1) {
        fprintf(stderr, "Error actualizando el superbloque\n");
        return -1;
    }

    return 0;
}

/*Funcion para inicializar el array de inodos*/
int initAI()
{

    struct inodo inodos[BLOCKSIZE / INODOSIZE];
    struct superbloque SB;
    int contInodos;

    // leemos el superbloque //
    if (bread(posSB, &SB) == FALLO)
    {
        perror(RED "Error al leer el SB");
        printf(RESET);
        return FALLO;
    }

    contInodos = SB.posPrimerInodoLibre + 1; // si hemos inicializado SB.posPrimerInodoLibre = 0

    for (int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++)
    { // para cada bloque del AI
        // leer el bloque de inodos i en el dispositivo virtual
        if (bread(i, &inodos) == FALLO)
        {
            perror(RED "Error al leer el SB");
            printf(RESET);
            return FALLO;
        }

        for (int j = 0; j < BLOCKSIZE / INODOSIZE; j++)
        {                         // para cada inodo del bloque
            inodos[j].tipo = 'l'; // libre
            if (contInodos < SB.totInodos)
            {                                               // si no hemos llegado al último inodo del AI
                inodos[j].punterosDirectos[0] = contInodos; // enlazamos con el siguiente
                contInodos++;
            }
            else
            { // hemos llegado al último inodo
                inodos[j].punterosDirectos[0] = UINT_MAX;
                break;
                // hay que salir del bucle, el último bloque no tiene por qué estar completo !!!
            }
        }

        // escribimos el bloque de inodos en el disco virtual//
        if (bwrite(i, &inodos) == FALLO)
        {
            perror(RED "Error al escribir AI en el SB");
            printf(RESET);
            return FALLO;
        }
    }

    return 0;
}

int escribir_bit(unsigned int nbloque, unsigned int bit)
{

    struct superbloque SB;
    unsigned char bufferMB[BLOCKSIZE];

    // leemos el superbloque //
    if (bread(posSB, &SB) == FALLO)
    {
        perror(RED "Error");
        printf(RESET);
        return FALLO;
    }

    int posbyte = nbloque / 8;
    int posbit = nbloque % 8;
    int nbloqueMB = posbyte / BLOCKSIZE;
    int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;

    // leemos el byte del mapa de bits//
    if (bread(nbloqueabs, bufferMB) == FALLO)
    {

        perror(RED "Error al leer byte ");
        printf(RESET);
        return FALLO;
    }

    // actualizamos la posicion de byte//
    posbyte = posbyte % BLOCKSIZE;

    unsigned char mascara = 128; // 10000000
    mascara >>= posbit;          // desplazamiento de bits a la derecha

    if (bit == 1)
    {
        bufferMB[posbyte] |= mascara; // ponemos bit a 1
    }
    else
    {
        bufferMB[posbyte] &= ~mascara; // ponemos bit a 0
    }

    // escribimos el byte modificado en el disco virtual//
    if (bwrite(nbloqueabs, bufferMB) == FALLO)
    {

        perror(RED "Error al escribir byte en disco virtual");
        printf(RESET);
        return FALLO;
    }

    return 0;
}

char leer_bit(unsigned int nbloque)
{

    // repetimos proceso como en escribir bit//
    struct superbloque SB;
    unsigned char bufferMB[BLOCKSIZE];

    // leemos el superbloque //
    if (bread(posSB, &SB) == FALLO)
    {
        perror(RED "Error");
        printf(RESET);
        return FALLO;
    }

    int posbyte = nbloque / 8;
    int posbit = nbloque % 8;
    int nbloqueMB = posbyte / BLOCKSIZE;
    int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;

    // leemos el byte del Mapa de bits //
    if (bread(nbloqueabs, bufferMB) == FALLO)
    {

        perror(RED "Error");
        printf(RESET);
        return FALLO;
    }

    unsigned char mascara = 128;  // 10000000
    mascara >>= posbit;           // desplazamiento de bits a la derecha, los que indique posbit
    mascara &= bufferMB[posbyte]; // operador AND para bits
    mascara >>= (7 - posbit);     // desplazamiento de bits a la derecha
                                  // para dejar el 0 o 1 en el extremo derecho y leerlo en decimal
    return mascara;
}

int reservar_bloque()
{

    struct superbloque SB;
    // leemos el superbloque //
    if (bread(posSB, &SB) == FALLO)
    {
        perror("Error al leer el superbloque");
        return FALLO;
    }
    // comrpobamos que haya bloques libres//
    if (SB.cantBloquesLibres == 0)
    {
        perror("No quedan bloques libres");
        return FALLO;
    }

    unsigned char bufferMB[BLOCKSIZE];  // buffer para leer los bloques de MB
    unsigned char bufferAux[BLOCKSIZE]; // buffer para comparar los bloques y encontrar uno no ocupado//
    memset(bufferAux, 255, BLOCKSIZE);  // inicializamos el buffer //

    int nbloqueMB;

    // leemos los bloques del MB para encontrar alguno que tenga un 0
    for (nbloqueMB = SB.posPrimerBloqueMB; nbloqueMB <= SB.posUltimoBloqueMB; nbloqueMB++)
    {
        if (bread(nbloqueMB, bufferMB) == FALLO)
        {
            perror("Error al leer el MB");
            return FALLO;
        }

        // hacemos la comparación
        if (memcmp(bufferMB, bufferAux, BLOCKSIZE) != 0)
        {
            break;
        }
    }

    int posbyte;
    // recorremos los bytes para encontrar uno con un 0
    for (posbyte = 0; posbyte < BLOCKSIZE; posbyte++)
    {
        if (bufferMB[posbyte] != 255)
        {
            break;
        }
    }

    int posbit = 0;
    unsigned char mascara = 128;
    // Recorremos los bits para encontrar  el 0
    while (bufferMB[posbyte] & mascara)
    {
        bufferMB[posbyte] <<= 1;
        posbit++;
    }

    // Determinamos el numero del bloque a reservar y escribimos un 1 en la posicion correspondiente del MB
    int nbloque = (nbloqueMB - SB.posPrimerBloqueMB) * BLOCKSIZE * 8 + (posbyte * 8) + posbit;
    escribir_bit(nbloque, 1);

    // actualizamos bloques libres
    SB.cantBloquesLibres--;
    bwrite(posSB, &SB);

    unsigned char bufferDatos[BLOCKSIZE];
    memset(bufferDatos, 0, BLOCKSIZE);
    bwrite(SB.posPrimerBloqueDatos + nbloque, bufferDatos);

    return nbloque;
}

int liberar_bloque(unsigned int nbloque)
{

    struct superbloque SB;
    // leemos el superbloque //
    if (bread(posSB, &SB) == FALLO)
    {
        perror("Error al leer el superbloque");
        return FALLO;
    }

    // Escribimos un 0
    escribir_bit(nbloque, 0);

    // actualizamos bloques libres
    SB.cantBloquesLibres++;
    bwrite(posSB, &SB);

    return nbloque;
}

int escribir_inodo(unsigned int ninodo, struct inodo *inodo){

    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE/INODOSIZE];

    // leemos el superbloque //
    if (bread(posSB, &SB) == FALLO)
    {
        perror("Error al leer el superbloque");
        return FALLO;
    }


    int nbloqueAI= (ninodo*INODOSIZE)/BLOCKSIZE;
    //obtenemos la posicion absoluta del bloque//
    int nbloqueabs = nbloqueAI+SB.posPrimerBloqueAI;

    bread(nbloqueabs,inodos);

    int posinodo = ninodo % (BLOCKSIZE / INODOSIZE);

    inodos[posinodo] = *inodo;

    bwrite(nbloqueabs, inodos);

    return 0;

}

int leer_inodo(unsigned int ninodo, struct inodo *inodo){


    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE/INODOSIZE];

    // leemos el superbloque //
    if (bread(posSB, &SB) == FALLO)
    {
        perror("Error al leer el superbloque");
        return FALLO;
    }

    int nbloqueAI= (ninodo*INODOSIZE)/BLOCKSIZE;
    //obtenemos la posicion absoluta del bloque//
    int nbloqueabs = nbloqueAI+SB.posPrimerBloqueAI;

    //leemos el bloque de inodos
    bread(nbloqueabs,inodos);

    //obtenemos la posicion del inodo dentro del buffer
    int posinodo = ninodo % (BLOCKSIZE / INODOSIZE);

    *inodo = inodos[posinodo];

    return 0;
}

int reservar_inodo(unsigned char tipo, unsigned char permisos){

    struct superbloque SB;
    struct inodo inodoReservado;
    int posInodoReservado;

    // leemos el superbloque //
    if (bread(posSB, &SB) == FALLO)
    {
        perror("Error al leer el superbloque");
        return FALLO;
    }
    // comrpobamos que haya inodos libres//
    if (SB.cantInodosLibres == 0)
    {
        perror("No quedan inodos libres");
        return FALLO;
    }

    posInodoReservado= SB.posPrimerInodoLibre;
    //Actualizamos posicion del primer inodo libre
    SB.posPrimerInodoLibre++;
  
    //leemos el inodoReservado
    if (leer_inodo(posInodoReservado, &inodoReservado) == FALLO) {
        perror("Error");
        return FALLO;
    }

     // Actualizar la lista enlazada de inodos libres
     SB.posPrimerInodoLibre = inodoReservado.punterosDirectos[0];

    //inicializamos los campos del inodo reservado
    inodoReservado.tipo = tipo;
    inodoReservado.permisos=permisos;
    inodoReservado.nlinks=1;
    inodoReservado.tamEnBytesLog=0;
    inodoReservado.atime = time(NULL);
    inodoReservado.btime = time(NULL);
    inodoReservado.ctime = time(NULL);
    inodoReservado.mtime = time(NULL);
    inodoReservado.numBloquesOcupados=0;
    memset(inodoReservado.punterosDirectos, 0, sizeof(inodoReservado.punterosDirectos));
    memset(inodoReservado.punterosIndirectos, 0, sizeof(inodoReservado.punterosIndirectos));
   
    //escribimos el inodo
    if(escribir_inodo(posInodoReservado,&inodoReservado)==FALLO){
        perror("Error");
        return FALLO;

    }
    //Actualizamos la cantidad de inodos
    SB.cantInodosLibres--;
    bwrite(posSB, &SB);
    return posInodoReservado;

}

int obtener_nRangoBL(struct inodo *inodo, unsigned int nblogico, unsigned int *ptr) {
    if (nblogico < DIRECTOS) {
        *ptr = inodo->punterosDirectos[nblogico];
        return 0;
    } else if (nblogico < INDIRECTOS0) {
        *ptr = inodo->punterosIndirectos[0];
        return 1;
    } else if (nblogico < INDIRECTOS1) {
        *ptr = inodo->punterosIndirectos[1];
        return 2;
    } else if (nblogico < INDIRECTOS2) {
        *ptr = inodo->punterosIndirectos[2];
        return 3;
    } else {
        *ptr = 0;
        fprintf(stderr, "Error: Bloque lógico fuera de rango\n");
        return -1;
    }
}

int obtener_indice(unsigned int nblogico, int nivel_punteros) {
    if (nblogico < DIRECTOS) {
        return nblogico;  // Caso de bloques directos
    } else if (nblogico < INDIRECTOS0) {
        return nblogico - DIRECTOS;  // Caso de indirecto simple
    } else if (nblogico < INDIRECTOS1) {
        if (nivel_punteros == 2) {
            return (nblogico - INDIRECTOS0) / NPUNTEROS;  // Índice de nivel 2
        } else if (nivel_punteros == 1) {
            return (nblogico - INDIRECTOS0) % NPUNTEROS;  // Índice de nivel 1
        }
    } else if (nblogico < INDIRECTOS2) {
        if (nivel_punteros == 3) {
            return (nblogico - INDIRECTOS1) / (NPUNTEROS * NPUNTEROS);  // Índice de nivel 3
        } else if (nivel_punteros == 2) {
            return ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) / NPUNTEROS;  // Índice de nivel 2
        } else if (nivel_punteros == 1) {
            return ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) % NPUNTEROS;  // Índice de nivel 1
        }
    }

    return -1;  // Error: fuera de rango
}

/**
 * Traduce un bloque lógico a un bloque físico en un inodo.
 */
 int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, unsigned char reservar) {
    struct inodo inodo;
    unsigned int ptr = 0, ptr_ant = 0, salvar_inodo = 0;
    int nRangoBL, nivel_punteros, indice;
    unsigned int buffer[NPUNTEROS];

    if (leer_inodo(ninodo, &inodo) < 0) return -1;

    nRangoBL = obtener_nRangoBL(&inodo, nblogico, &ptr);
    if (nRangoBL == -1) return -1; // Error: bloque fuera de rango

    nivel_punteros = nRangoBL;
    while (nivel_punteros > 0) {
        if (ptr == 0) { // No existe el bloque de punteros
            if (reservar == 0) return -1;
            ptr = reservar_bloque();
            if (ptr == -1) return -1;
            memset(buffer, 0, BLOCKSIZE);
            inodo.numBloquesOcupados++;
            inodo.ctime = time(NULL);
            salvar_inodo = 1;

            if (nivel_punteros == nRangoBL) {
                inodo.punterosIndirectos[nRangoBL - 1] = ptr;
            } else {
                buffer[indice] = ptr;
                if (bwrite(ptr_ant, buffer) < 0) return -1;
            }
        } else {
            if (bread(ptr, buffer) < 0) return -1;
        }

        indice = obtener_indice(nblogico, nivel_punteros);
        ptr_ant = ptr;
        ptr = buffer[indice];
        nivel_punteros--;
    }

    if (ptr == 0) { // No existe bloque de datos
        if (reservar == 0) return -1;
        ptr = reservar_bloque();
        if (ptr == -1) return -1;
        inodo.numBloquesOcupados++;
        inodo.ctime = time(NULL);
        salvar_inodo = 1;

        if (nRangoBL == 0) {
            inodo.punterosDirectos[nblogico] = ptr;
        } else {
            buffer[indice] = ptr;
            if (bwrite(ptr_ant, buffer) < 0) return -1;
        }
    }

    if (salvar_inodo) escribir_inodo(ninodo, &inodo);
    return ptr;
}

// Funcion que devuelve valor decimal de los bits sobrantes//
int bitSobrantes(int n)
{
    int sol;
    for (int i = 7; n > 0; n--,i--)
    {
        sol += 2 << i;
        
    }
    return sol;
}