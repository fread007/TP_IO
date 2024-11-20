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
    //ont verifie que les entree sont valide
    if (f == NULL || f->buffer == NULL || f->mode != 'E') {
        return 1;
    }

    //initialisation des arguments
    va_list args;
    va_start(args, format);

    
    int nbr_ecrit = 0;  //nombre d'element ecrit
    char buffer[10];    //buffer pour transformer les int , la taille est de 10 car un int ne depasse pas 10 chiffres

    //ont parcours la string format
    while(*format != '\0'){
        if(*format == '%'){ //cas ou l'ont on a un element a inserer
            format++;   //ont regarde qelle est le type de l'element
            switch(*format){
                case 'c':   //char
                    char actuel = (char) va_arg(args, int);   //ont recupere l'element
                    ecrire(&actuel, 1, 1, f);   //ont l'ecrit
                    nbr_ecrit++;
                    break;
                case 's':   //string
                    char * string = va_arg( args, char * );  //ont recupere l'element
                    while(*string != '\0'){   //ont ecrit chaque caractere de la string
                        ecrire(string, 1, 1, f);
                        nbr_ecrit++;
                        string++;
                    }
                    break;
                case 'd':   //int
                    int i = va_arg(args, int);  //ont recupere l'element
                    int length = int_to_string(i,buffer);   //ont transforme l'element en string
                    ecrire(buffer, sizeof(char), length, f);    //ont l'ecrit
                    nbr_ecrit += length;
                    break;
                default:
                    return 1;
            }
        }
        else{
            //cas ou l'ont traite un caractere simple
            ecrire(format, sizeof(char), 1, f);
        }
        format++;
    }
 
    //ont libere les arguments
    va_end(args);
    return nbr_ecrit;
}


/* directly in stdout */
int ecriref (const char *format, ...){

    //initialisation des arguments
    va_list args;
    va_start(args, format);

    
    int nbr_ecrit = 0;  //nombre d'element ecrit
    char buffer[10];    //buffer pour transformer les int , la taille est de 10 car un int ne depasse pas 10 chiffres

    while(*format != '\0'){ //ont parcours la string format
        if(*format == '%'){ //cas ou l'ont on a un element a inserer
            format++;
            switch(*format){    //ont regarde qelle est le type de l'element
                case 'c':   //char
                    char actuel = (char) va_arg(args, int);  //ont recupere l'element
                    ecrire(&actuel, 1, 1, stdout);  //ont l'ecrit
                    nbr_ecrit++;
                    break;
                case 's':   //string
                    char * string = va_arg( args, char * ); //ont recupere l'element
                    while(*string != '\0'){   //ont ecrit chaque caractere de la string
                        ecrire(string, 1, 1, stdout);   //ont l'ecrit
                        nbr_ecrit++;    
                        string++;
                    }
                    break;
                case 'd':   //int
                    int i = va_arg(args, int);  //ont recupere l'element
                    int length = int_to_string(i,buffer);   //ont transforme l'element en string
                    ecrire(buffer, sizeof(char), length, stdout);   //ont l'ecrit
                    nbr_ecrit += length;
                    break;
                default:
                    return 1;
            }
        }
        else{
            //cas ou l'ont traite un caractere simple
            ecrire(format, sizeof(char), 1, stdout);
        }
        format++;
    }

    //ont libere les arguments
    va_end(args);
    return nbr_ecrit;
}


int fliref (FICHIER *f, const char *format, ...) {
    
    if(f == NULL || f->buffer == NULL || f->mode != 'L'){   //ont verifie que les entree sont valide
        return 0;
    }

    //initialisation des arguments
    va_list args;
    va_start(args, format);

    
    int nbr_lecture = 0;    //nombre d'element lu
    char tmp;   
    int lu = -1;    //permet de savoir si l'ont a lu un caractere
    int flag = 0;   //permer de savoir si ont a lu un caractere en plus

    while(*format != '\0'){ //ont parcours la string format
        if(*format == '%'){ //cas ou l'ont on a un element a recuperer
            format++;
            switch(*format){    //ont regarde qelle est le type de l'element
                case 'c' :  //char
                    char *c = va_arg(args, char *); //ont recupere l'emplacement ou ont doit inserer l'element
                    if(flag == 1){  //si l'ont a deja lu le caractere ont l'insere
                        *c = tmp;
                        flag = 0;
                    }
                    else if(lire(&tmp, 1, 1, f) == 1){  //sinon ont le lit
                        nbr_lecture++;
                    }
                    else {  //si il na pas pu etre lu ont arrte
                        return nbr_lecture;
                    }
                break;
                case 's' :  //string
                    char *s = va_arg(args, char *); //ont recupere l'emplacement ou ont doit inserer l'element
                    int i = 0;  //index du tableau
                    lu = -1;
                    if(flag == 1 && tmp != ' ' && tmp != '\n' && tmp != '\0' && tmp != '\t'){   //si l'ont a deja lu un caractere ont l'insere
                        s[i] = tmp;
                        i++;
                    }
                    while((lu=lire(&tmp, 1, 1, f)) == 1 && tmp != ' ' && tmp != '\n' && tmp != '\0' && tmp != '\t'){    //ont li les caracetere jusqu'a un separateur
                        s[i] = tmp;
                        i++;
                    }
                    s[i] = '\0';    //ont ajoute le caractere de fin de string
                    nbr_lecture++;
                    if(lu == 0 || *format == '\0'){ //si ont est a la fin du fichier ou de la string ont arrete
                        return nbr_lecture;
                    }
                    flag = 1;   //ont indique que l'ont a lu un caractere en plus
                break;
                case 'd' :  //int
                    int *integer = va_arg(args, int *);  //ont recupere l'emplacement ou ont doit inserer l'element
                    char buffer[11];    //buffer pour transformer les int , la taille est de 11 car un int ne depasse pas 10 chiffres et il faut un caractere de fin de string
                    lu = -1;
                    int ctr = 0;
                    if(flag == 1 && tmp <= '9' && tmp >= '0'){  //si l'ont a deja lu un caracterent et que c'est un chiffre ont l'insere
                        buffer[ctr] = tmp;
                        ctr++;
                    }
                    while((lu = lire(&tmp, 1, 1, f)) == 1 &&  tmp <= '9' && tmp >= '0'){    //ont li les chiffres
                        buffer[ctr] = tmp;
                        ctr++;
                    }
                    buffer[ctr] = '\0'; //ont ajoute le caractere de fin de string
                    *integer = atoi(buffer);    //ont transforme la string en int
                    nbr_lecture++;
                    if(lu == 0 || *format == '\0'){ //si ont est a la fin du fichier ou de la string ont arrete
                        return nbr_lecture;
                    }
                    flag = 1;   //ont indique que l'ont a lu un caractere en plus
                    
                break;
                default:
                    return 1;
                
            }
        }
        else{
            //cas ou l'ont traite un caractere simple
            if(flag == 1){  //si on a deja lu un caractere 
                flag = 0;
            }
            else{   //sinon ont lit le caractere
                lire(&tmp, 1, 1, f);
            }
            if(tmp != *format){ //ont verifie que le caractere lu est bien celui attendu
                return nbr_lecture;
            }
        }
        
        format++;
    }

    //ont libere les arguments
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