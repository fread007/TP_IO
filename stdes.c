#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 
#include "stdes.h"


#define TAILLE_BUFFER 1000

__attribute__((constructor)) void init(){
    stdout = malloc(sizeof(FICHIER));
    stderr = malloc(sizeof(FICHIER));
    stdout->descipteur = 1;
    stderr->descipteur = 2;
    stdout->mode = 'W';
    stderr->mode = 'W';
    stdout->buffer = malloc(TAILLE_BUFFER);
    stderr->buffer = malloc(TAILLE_BUFFER);
    stdout->index = 0;
    stderr->index = 0;
    stdout->nbrOctets = 0;
    stderr->nbrOctets = 0;
}

FICHIER *ouvrir(const char *nom, char mode){
    if(nom == NULL || (mode != 'W' && mode != 'R')){   //ont verifie que les entree sont valide
        return NULL;
    }

    //ont aloue la structure de base
    FICHIER *f = malloc(sizeof(FICHIER));
    if(f == NULL){
        return NULL;
    }

    //ont ouvre le fichier avec le bon mode
    if(mode == 'E'){
        f->descipteur = open(nom,O_WRONLY | O_CREAT | O_TRUNC, 0666);
    }
    else{
        f->descipteur = open(nom,O_RDONLY);
    }
    //ont verifie si le fichier a bien etais ouvert
    if(f->descipteur == -1){
        free(f);
        return NULL;
    }

    //ont alloue de la memoire pour le buffer
    f->buffer = malloc(TAILLE_BUFFER);
    //ont verifie que la memoire a bien etais allouer
    if(f->buffer == NULL){
        close(f->descipteur);
        free(f);
        return NULL;
    }

    //ont insert le mode du fichier et les index urile
    f->mode = mode;
    f->index = 0;
    f->nbrOctets = 0;

    return f;
}

int lire(void *p, unsigned int taille, unsigned int nbelem, FICHIER *f){
    if(f == NULL || f->buffer == NULL || f->mode != 'L'){   //ont verifie que les entree sont valide
        return 0;
    }

    //ont remplis le buffer pocede au moin un element
    if(f->nbrOctets == 0 || f->index + taille > f->nbrOctets){
        memcpy(f->buffer,f->buffer + f->index,f->nbrOctets - f->index); //ont decale les octets non lu
        f->nbrOctets = f->nbrOctets - f->index; //ont met a jour le nombre d'octets non lu
        //ont remplis la suite du buffer
        int tmp = 0 ;
        while(f->nbrOctets != TAILLE_BUFFER && tmp > 0){
            f->nbrOctets += tmp;
            tmp = read(f->descipteur,&f->buffer[f->nbrOctets],TAILLE_BUFFER - f->nbrOctets);
        }
        f->index = 0;
    }

    //lecture des elements
    int lu = 0;
    while(lu < nbelem && f->index + taille <= f->nbrOctets){    //tant que l'ont a pas lu tout les elements ou que l'ont a pas atteint la fin du buffer
        memcpy(p+lu,&f->buffer[f->index],taille);
        lu++;
        f->index += taille;
    }

    return lu;

}


int ecrire(const void *p, unsigned int taille, unsigned int nbelem, FICHIER *f){
    return 0;
}
int vider(FICHIER *f){
    return 0;
}

int fecriref (FICHIER *f, const char *format, ...){
    return 0;
}
/* directly in stdout */
int ecriref (const char *format, ...){
    return 0;
}
int fliref (FICHIER *f, const char *format, ...) {
    return 0;
}

int fermer(FICHIER *f){
    //ont verifie que l'entree est valide
    if(f == NULL){
        return 0;
    }
    //ont verifie que le buffer est bien alouer
    if(f->buffer != NULL){
        free(f->buffer);    //liberation du buffer
    }
    //liberaton de la structure
    free(f);

    return 0;
}