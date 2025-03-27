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


    bmount(argv[1]);
    bread(posSB,&SB);

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
   int nbloqueMB;
    fprintf(stderr, BLUE "****Primer y ultimo bit del MB de cada zona del dispositivo****\n\n");
    leer_bit(0);

     // primer bit SB
    posbyte =  posSB / 8;
    posbit = posSB % 8;
    nbloqueMB = posbyte / BLOCKSIZE;
    nbloqueabs = SB.posPrimerBloqueMB +nbloqueMB;
   
    fprintf(stderr, GRAY "[leer_bit(%d)→ posbyte:%d, posbyte (ajustado): , posbit:%d, nbloqueMB:%d, nbloqueabs:%d)]\n",posSB,posbyte,posbit,nbloqueMB,nbloqueabs);
    fprintf(stderr, BLUE "posSB: %d → leer_bit(%d) = %d\n",posSB, posSB,leer_bit(0));

    //primer bit MB
    posbyte =  SB.posPrimerBloqueMB / 8;
    posbit = SB.posPrimerBloqueMB % 8;
    nbloqueMB = posbyte / BLOCKSIZE;
    nbloqueabs = SB.posPrimerBloqueMB +nbloqueMB;

    fprintf(stderr, GRAY "[leer_bit(%d)→ posbyte:%d, posbyte (ajustado): , posbit:%d, nbloqueMB:%d, nbloqueabs:%d)]\n",SB.posPrimerBloqueMB,posbyte,posbit,nbloqueMB,nbloqueabs);
    fprintf(stderr, BLUE "SB.posPrimerBloqueMB: %d → leer_bit(%d) = %d\n",SB.posPrimerBloqueMB,SB.posPrimerBloqueMB ,leer_bit(SB.posPrimerBloqueMB));

    //ultimo bit MB
    posbyte =  SB.posUltimoBloqueMB / 8;
    posbit = SB.posUltimoBloqueMB % 8;
    nbloqueMB = posbyte / BLOCKSIZE;
    nbloqueabs = SB.posPrimerBloqueMB +nbloqueMB;

    fprintf(stderr, GRAY "[leer_bit(%d)→ posbyte:%d, posbyte (ajustado): , posbit:%d, nbloqueMB:%d, nbloqueabs:%d)]\n",SB.posUltimoBloqueMB,posbyte,posbit,nbloqueMB,nbloqueabs);
    fprintf(stderr, BLUE "SB.posUltimoBloqueMB: %d → leer_bit(%d) = %d\n",SB.posUltimoBloqueMB,SB.posUltimoBloqueMB ,leer_bit(SB.posUltimoBloqueMB));

    //primer bit AI
    posbyte =  SB.posPrimerBloqueAI / 8;
    posbit = SB.posPrimerBloqueAI % 8;
    nbloqueMB = posbyte / BLOCKSIZE;
    nbloqueabs = SB.posPrimerBloqueMB +nbloqueMB;
   
    fprintf(stderr, GRAY "[leer_bit(%d)→ posbyte:%d, posbyte (ajustado): , posbit:%d, nbloqueMB:%d, nbloqueabs:%d)]\n",SB.posPrimerBloqueAI,posbyte,posbit,nbloqueMB,nbloqueabs);
    fprintf(stderr, BLUE "SB.posPrimerBloqueAI: %d → leer_bit(%d) = %d\n",SB.posPrimerBloqueAI,SB.posPrimerBloqueAI ,leer_bit(SB.posPrimerBloqueAI));

    //ultimo bit AI
    posbyte =  SB.posUltimoBloqueAI / 8;
    posbit = SB.posUltimoBloqueAI % 8;
    nbloqueMB = posbyte / BLOCKSIZE;
    nbloqueabs = SB.posPrimerBloqueMB +nbloqueMB;

    fprintf(stderr, GRAY "[leer_bit(%d)→ posbyte:%d, posbyte (ajustado): , posbit:%d, nbloqueMB:%d, nbloqueabs:%d)]\n",SB.posUltimoBloqueAI,posbyte,posbit,nbloqueMB,nbloqueabs);
    fprintf(stderr, BLUE "SB.posUltimoBloqueAI: %d → leer_bit(%d) = %d\n",SB.posUltimoBloqueAI,SB.posUltimoBloqueAI ,leer_bit(SB.posUltimoBloqueAI));

    //primer bit bloqueDatos
    posbyte =  SB.posPrimerBloqueDatos / 8;
    posbit = SB.posPrimerBloqueDatos % 8;
    nbloqueMB = posbyte / BLOCKSIZE;
    nbloqueabs = SB.posPrimerBloqueMB +nbloqueMB;

    fprintf(stderr, GRAY "[leer_bit(%d)→ posbyte:%d, posbyte (ajustado): , posbit:%d, nbloqueMB:%d, nbloqueabs:%d)]\n",SB.posPrimerBloqueDatos,posbyte,posbit,nbloqueMB,nbloqueabs);
    fprintf(stderr, BLUE "SB.posPrimerBloqueDatos: %d → leer_bit(%d) = %d\n",SB.posPrimerBloqueDatos,SB.posPrimerBloqueDatos ,leer_bit(SB.posPrimerBloqueDatos));
    
    //ultimo bit bloqueDatos
    posbyte =  SB.posUltimoBloqueDatos / 8;
    posbit = SB.posUltimoBloqueDatos % 8;
    nbloqueMB = posbyte / BLOCKSIZE;
    nbloqueabs = SB.posPrimerBloqueMB +nbloqueMB;

    fprintf(stderr, GRAY "[leer_bit(%d)→ posbyte:%d, posbyte (ajustado): , posbit:%d, nbloqueMB:%d, nbloqueabs:%d)]\n",SB.posUltimoBloqueDatos,posbyte,posbit,nbloqueMB,nbloqueabs);
    fprintf(stderr, BLUE "SB.posUltimoBloqueDatos: %d → leer_bit(%d) = %d\n\n",SB.posUltimoBloqueDatos,SB.posUltimoBloqueDatos ,leer_bit(SB.posUltimoBloqueDatos));

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

    printf(RESET);
}
