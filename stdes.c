#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 
#include "stdes.h"
#include <stdarg.h>


#define TAILLE_BUFFER 1000

__attribute__((constructor)) void init(){
    stdout = malloc(sizeof(FICHIER));
    stderr = malloc(sizeof(FICHIER));
    stdout->descipteur = 1;
    stderr->descipteur = 2;
    stdout->mode = 'E';
    stderr->mode = 'E';
    stdout->buffer = malloc(TAILLE_BUFFER);
    stderr->buffer = malloc(TAILLE_BUFFER);
    stdout->index = 0;
    stderr->index = 0;
    stdout->nbrOctets = 0;
    stderr->nbrOctets = 0;
}

FICHIER *ouvrir(const char *nom, char mode){
    
    if(nom == NULL || (mode != 'L' && mode != 'E')){   //ont verifie que les entree sont valide
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
        tmp = read(f->descipteur,&f->buffer[f->nbrOctets],TAILLE_BUFFER - f->nbrOctets);
        if(tmp > 0){
            f->nbrOctets += tmp;
        }
        while(f->nbrOctets != TAILLE_BUFFER && tmp>0){
            tmp = read(f->descipteur,&f->buffer[f->nbrOctets],TAILLE_BUFFER - f->nbrOctets);
            if(tmp > 0){
                f->nbrOctets += tmp;
            } 
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
    if(f == NULL || f->buffer == NULL || f->mode != 'E'){   //ont verifie que les entree sont valide
        return 0;
    }

    //ont verifie que le buffer a assez de place pour les elements
    if(f->index + taille > TAILLE_BUFFER){
        write(f->descipteur,f->buffer ,f->nbrOctets); //ont ecrit le buffer
        f->index = 0;
        f->nbrOctets = 0;
    }

    //ont ecrit les elements dans le buffer
    int ecrit = 0;
    while(ecrit < nbelem && f->index + taille <= TAILLE_BUFFER){    //tant que l'ont a pas ecrit tout les elements ou que l'ont a pas atteint la fin du buffer
        memcpy(&f->buffer[f->index],p+(ecrit*taille),taille);
        ecrit++;
        f->index += taille;
        f->nbrOctets += taille;
    }

    return ecrit;

}

int vider(FICHIER *f){
    if(f == NULL || f->buffer == NULL || f->mode != 'E'){   //ont verifie que les entree sont valide
        return 1;
    }

    //ont ecrit le buffer
    write(f->descipteur,f->buffer,f->nbrOctets );
    f->index = 0;
    f->nbrOctets = 0;

    return 0;
}

int fecriref (FICHIER *f, const char *format, ...){
    if (f == NULL || f->buffer == NULL || f->mode != 'E') {
        return 1;
    }

    va_list args;
    va_start(args, format);

    
    int nbr_ecrit = 0;
    char buffer[10];

    while(*format != '\0'){
        if(*format == '%'){
            format++;
            switch(*format){
                case 'c':
                    char actuel = (char) va_arg(args, int);
                    ecrire(&actuel, 1, 1, f);
                    nbr_ecrit++;
                    break;
                case 's':
                    char * string = va_arg( args, char * );
                    while(*string != '\0'){
                        ecrire(string, 1, 1, f);
                        nbr_ecrit++;
                        string++;
                    }
                    break;
                case 'd':
                    int i = va_arg(args, int);
                    int length = int_to_string(i,buffer);
                    ecrire(buffer, sizeof(char), length, f);
                    nbr_ecrit += length;
                    break;
                default:
                    return 1;
            }
        }
        else{
            ecrire(format, sizeof(char), 1, f);
        }
        format++;
    }
 
    va_end(args);
    return nbr_ecrit;
}
/* directly in stdout */
int ecriref (const char *format, ...){

    va_list args;
    va_start(args, format);

    
    int nbr_ecrit = 0;
    char buffer[10];

    while(*format != '\0'){
        if(*format == '%'){
            format++;
            switch(*format){
                case 'c':
                    char actuel = (char) va_arg(args, int);
                    ecrire(&actuel, 1, 1, stdout);
                    nbr_ecrit++;
                    break;
                case 's':
                    char * string = va_arg( args, char * );
                    while(*string != '\0'){
                        ecrire(string, 1, 1, stdout);
                        nbr_ecrit++;
                        string++;
                    }
                    break;
                case 'd':
                    int i = va_arg(args, int);
                    int length = int_to_string(i,buffer);
                    ecrire(buffer, sizeof(char), length, stdout);
                    nbr_ecrit += length;
                    break;
                default:
                    return 1;
            }
        }
        else{
            ecrire(format, sizeof(char), 1, stdout);
        }
        format++;
    }

    va_end(args);
    return nbr_ecrit;
}


int fliref (FICHIER *f, const char *format, ...) {
    
    va_list args;
    va_start(args, format);

    
    int nbr_lecture = 0;
    char tmp;
    int lu = -1;
    int flag = 0;

    while(*format != '\0'){
        if(*format == '%'){
            format++;
            switch(*format){
                case 'c' :
                    char *c = va_arg(args, char *);
                    if(flag == 1){
                        *c = tmp;
                        flag = 0;
                    }
                    else if(lire(&tmp, 1, 1, f) == 1){
                        nbr_lecture++;
                    }
                    else {
                        return nbr_lecture;
                    }
                break;
                case 's' :
                    vider(stdout);
                    char *s = va_arg(args, char *);
                    int i = 0;
                    lu = -1;
                    if(flag == 1 && tmp != ' ' && tmp != '\n' && tmp != '\0' && tmp != '\t'){
                        s[i] = tmp;
                        i++;
                    }
                    while((lu=lire(&tmp, 1, 1, f)) == 1 && tmp != ' ' && tmp != '\n' && tmp != '\0' && tmp != '\t'){
                        s[i] = tmp;
                        i++;
                    }
                    s[i] = '\0';
                    nbr_lecture++;
                    if(lu == 0 || *format == '\0'){
                        return nbr_lecture;
                    }
                    flag = 1;
                break;
                case 'd' :
                    int *integer = va_arg(args, int *);
                    char buffer[10];
                    lu = -1;
                    int ctr = 0;
                    if(flag == 1 && tmp <= '9' && tmp >= '0'){
                        buffer[ctr] = tmp;
                        ctr++;
                    }
                    while((lu = lire(&tmp, 1, 1, f)) == 1 &&  tmp <= '9' && tmp >= '0'){
                        buffer[ctr] = tmp;
                        ctr++;
                    }
                    buffer[ctr] = '\0';
                    *integer = atoi(buffer);
                    nbr_lecture++;
                    if(lu == 0 || *format == '\0'){
                        return nbr_lecture;
                    }
                    flag = 1;
                    
                break;
                default:
                    return 1;
                
            }
        }
        else{
            if(flag == 1){
                flag = 0;
            }
            else{
                lire(&tmp, 1, 1, f);
            }
            if(tmp != *format){
                return nbr_lecture;
            }
        }
        
        format++;
    }


    va_end(args);
    return nbr_lecture;
    
}

int fermer(FICHIER *f){
    //ont verifie que l'entree est valide
    if(f == NULL){
        return 0;
    }
    if(f->mode == 'E'){
        vider(f);   //ont vide le buffer
    }
    //ont verifie que le buffer est bien alouer
    if(f->buffer != NULL){
        free(f->buffer);    //liberation du buffer
    }
    //liberaton de la structure
    free(f);

    return 0;
}


int int_to_string(int number, char* tab){
    int p = number;
    int size = 0;

    if(number < 10){
        tab[0] = 48 + number;
        size += 1;
        return size;
    }
    while(p != 0){
        size += 1;
        p = p/10;
    }
    p = number;
    for(int i = size; i>0;i--){
        tab[i-1]= (p%10) + 48;
        p = p /10;
    }
    return size;
}