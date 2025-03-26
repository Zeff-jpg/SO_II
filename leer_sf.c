/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/

#include "fichero_basico.h"

int main(int argc, char **argv)
{

    struct superbloque SB;
    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];
    char btime[80];

    struct inodo inodo;
    int ninodo=0;

    int fs = open(argv[1], O_RDONLY);

    // Abrimos nuestro fichero donde se "almacena" el disco virtual//
    if (fs == FALLO)
    {
        perror(RED "Error al abrir el disco virtual");
        printf(RESET);
        return FALLO;
    }

    // leemos el superbloque //
    if (read(fs, &SB, BLOCKSIZE) == FALLO)
    {
        perror(RED "Error al leer el SB");
        printf(RESET);
        return FALLO;
    }

    if(close(fs)==FALLO){
        perror(RED "Error ");
        printf(RESET);
        return FALLO;
    }

    // imprimimos los datos del superbloque//
    printf(BLUE "****DATOS DEL SUPER BLOQUE****\n\n");

    fprintf(stderr, GRAY "posPrimerBloqueMB = %d\n", SB.posPrimerBloqueMB);
    fprintf(stderr, "posUltimoBloqueMB = %d\n", SB.posUltimoBloqueMB);
    fprintf(stderr, "posPrimerBloqueAI = %d\n", SB.posPrimerBloqueAI);
    fprintf(stderr, "posUltimoBloqueAI = %d\n", SB.posUltimoBloqueAI);
    fprintf(stderr, "posPrimerBloqueDatos = %d\n", SB.posPrimerBloqueDatos);
    fprintf(stderr, "posUltimoBloqueDatos = %d\n", SB.posUltimoBloqueDatos);
    fprintf(stderr, "posInodoRaiz = %d\n", SB.posInodoRaiz);

    fprintf(stderr, "posPrimerInodoLibre = %d\n", SB.posPrimerInodoLibre);
    fprintf(stderr, "cantBloquesLibres = %d\n", SB.cantBloquesLibres);
    fprintf(stderr, "cantInodosLibres = %d\n", SB.cantInodosLibres);
    fprintf(stderr, "totBloques = %d\n", SB.totBloques);
    fprintf(stderr, "totInodos = %d\n\n", SB.totInodos);

    fprintf(stderr, "sizeof struct superbloque= %ld\n", sizeof(struct superbloque));
    fprintf(stderr, "sizeof struct inodo = %ld\n\n", sizeof(struct inodo));

    /*
    // imprimimos el recorrido de la lista de inodos //
    fprintf(stderr, BLUE "****RECORRIDO LISTA ENLAZADA DE INODOS****\n\n");
    for (int i = 0; i < 10; i++)
    {
    }
    */
   int posbyte;
   int posbyteAjustado;
   int posbit;
   int nbloqueabs;
    fprintf(stderr, BLUE "****Primer y ultimo bit del MB de cada zona del dispositivo****\n\n");
    leer_bit(0);
/*
    nbloqueabs = SB.posPrimerBloqueMB + posSB;

    //leemos primer bit SB
    fprintf(stderr, GRAY "[leer_bit(%d)→ posbyte:0, posbyte (ajustado): , posbit:0, nbloqueMB:%d, nbloqueabs:%d)]\n",posSB,posSB,nbloqueabs);
    fprintf(stderr, BLUE "posSB: %d → leer_bit(%d) = %d\n",posSB, posSB,leer_bit(0));

    nbloqueabs = SB.posPrimerBloqueMB + posSB;
    posbyte =  SB.posPrimerBloqueMB / 8;
    posbit = SB.posPrimerBloqueMB % 8;

    //primer bit MB
    fprintf(stderr, GRAY "[leer_bit(%d)→ posbyte:%d, posbyte (ajustado): , posbit:%d, nbloqueMB:%d, nbloqueabs:%d)]\n",SB.posPrimerBloqueMB,posbyte,posbit,SB.posPrimerBloqueMB/BLOCKSIZE,nbloqueabs);
    fprintf(stderr, BLUE "SB.posPrimerBloqueMB: %d → leer_bit(%d) = %d\n",SB.posPrimerBloqueMB,SB.posPrimerBloqueMB ,leer_bit(SB.posPrimerBloqueMB));

 

    leer_inodo(ninodo, &inodo);
    ts = localtime(&inodo.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodo.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodo.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodo.btime);
    strftime(ctime, sizeof(btime), "%a %Y-%m-%d %H:%M:%S", ts);
    printf("ID: %d ATIME: %s MTIME: %s CTIME: %s BTIME: %s\\n", ninodo, atime, mtime, ctime, btime);
*/
    printf(RESET);
}
