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
    stdout->descripteur = 1;
    stderr->descripteur = 2;
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
    
    if(nom == NULL || (mode != 'L' && mode != 'E')){   //On vérifie que les entrées sont valides
        return NULL;
    }

    //On alloue la structure de base
    FICHIER *f = malloc(sizeof(FICHIER));
    if(f == NULL){
        return NULL;
    }

    //On ouvre le fichier avec le bon mode
    if(mode == 'E'){
        f->descripteur = open(nom,O_WRONLY | O_CREAT | O_TRUNC, 0666);
    }
    else{
        f->descripteur = open(nom,O_RDONLY);
    }
     //On vérifie si le fichier a bien été ouvert
    if(f->descripteur == -1){
        free(f);
        return NULL;
    }

    //On alloue de la mémoire pour le buffer
    f->buffer = malloc(TAILLE_BUFFER);
    //On vérifie que la mémoire a bien été allouée
    if(f->buffer == NULL){
        close(f->descripteur);
        free(f);
        return NULL;
    }

    //On insère le mode du fichier et initialise les index utiles
    f->mode = mode;
    f->index = 0;
    f->nbrOctets = 0;

    return f;
}

int lire(void *p, unsigned int taille, unsigned int nbelem, FICHIER *f){
    if(f == NULL || f->buffer == NULL || f->mode != 'L'){   //On vérifie que les entrées sont valides
        return 0;
    }

    //On remplit le buffer pour qu'il contienne au moins un élément
    if(f->nbrOctets == 0 || f->index + taille > f->nbrOctets){
        memcpy(f->buffer,f->buffer + f->index,f->nbrOctets - f->index); //On décale les octets non lus
        f->nbrOctets = f->nbrOctets - f->index; //On met à jour le nombre d'octets non lus
        //On remplit le reste du buffer
        int tmp = 0 ;
        tmp = read(f->descripteur,&f->buffer[f->nbrOctets],TAILLE_BUFFER - f->nbrOctets);
        if(tmp > 0){
            f->nbrOctets += tmp;
        }
        while(f->nbrOctets != TAILLE_BUFFER && tmp>0){
            tmp = read(f->descripteur,&f->buffer[f->nbrOctets],TAILLE_BUFFER - f->nbrOctets);
            if(tmp > 0){
                f->nbrOctets += tmp;
            } 
        }
        f->index = 0;
    }

    //Lecture des éléments
    int lu = 0;
    while(lu < nbelem && f->index + taille <= f->nbrOctets){    //Tant qu'on n'a pas lu tous les éléments ou atteint la fin du buffer
        
        memcpy(p+lu,&f->buffer[f->index],taille);
        lu++;
        f->index += taille;
    }

    return lu;

}


int ecrire(const void *p, unsigned int taille, unsigned int nbelem, FICHIER *f){
    if(f == NULL || f->buffer == NULL || f->mode != 'E'){   //On vérifie que les entrées sont valides
        return 0;
    }

    //On vérifie que le buffer a assez de place pour les éléments
    if(f->index + taille > TAILLE_BUFFER){
        write(f->descripteur,f->buffer ,f->nbrOctets); //On écrit le contenu du buffer
        f->index = 0;
        f->nbrOctets = 0;
    }

    //On écrit les éléments dans le buffer
    int ecrit = 0;
    while(ecrit < nbelem && f->index + taille <= TAILLE_BUFFER){    //Tant qu'on n'a pas écrit tous les éléments ou atteint la fin du buffer
        memcpy(&f->buffer[f->index],p+(ecrit*taille),taille);
        ecrit++;
        f->index += taille;
        f->nbrOctets += taille;
    }

    return ecrit;

}

int vider(FICHIER *f){
    if(f == NULL || f->buffer == NULL || f->mode != 'E'){   //On vérifie que les entrées sont valides
        return 1;
    }

    //On écrit le contenu du buffer
    write(f->descripteur,f->buffer,f->nbrOctets );
    f->index = 0;
    f->nbrOctets = 0;

    return 0;
}

int fecriref (FICHIER *f, const char *format, ...){
    //On vérifie que les entrées sont valides
    if (f == NULL || f->buffer == NULL || f->mode != 'E') {
        return 1;
    }

    //Initialisation des arguments
    va_list args;
    va_start(args, format);

    
    int nbr_ecrit = 0;  //Nombre d'éléments écrits
    char buffer[10];    //Buffer pour convertir les entiers, de taille 10 car un int ne dépasse pas 10 chiffres

    //On parcourt la chaîne de format
    while(*format != '\0'){
        if(*format == '%'){ //Cas où on rencontre un élément à insérer
            format++;   //On vérifie le type de l'élément
            switch(*format){
                case 'c':   //Char
                    char actuel = (char) va_arg(args, int);   //On récupère l'élément
                    ecrire(&actuel, 1, 1, f);   //On l'écrit
                    nbr_ecrit++;
                    break;
                case 's':   //String
                    char * string = va_arg( args, char * );  //On récupère l'élément
                    while(*string != '\0'){   //On écrit chaque caractère de la chaîne
                        ecrire(string, 1, 1, f);
                        nbr_ecrit++;
                        string++;
                    }
                    break;
                case 'd':   //Int
                    int i = va_arg(args, int);  //On récupère l'élément
                    int length = int_to_string(i,buffer);   //On convertit l'entier en chaîne de caractères
                    ecrire(buffer, sizeof(char), length, f);    //On l'écrit
                    nbr_ecrit += length;
                    break;
                default:
                    return 1;
            }
        }
        else{
            //Cas où on traite un caractère simple
            ecrire(format, sizeof(char), 1, f);
            nbr_ecrit ++;
        }
        format++;
    }
 
    //On libère les arguments
    va_end(args);
    return nbr_ecrit;
}


/* directly in stdout */
int ecriref (const char *format, ...){

    //Initialisation des arguments variables
    va_list args;
    va_start(args, format);

    
    int nbr_ecrit = 0;  //Nombre d'éléments écrits
    char buffer[10];    //Buffer pour convertir les entiers, de taille 10 car un entier ne dépasse pas 10 chiffres

    while(*format != '\0'){ //On parcourt la chaîne de format
        if(*format == '%'){ //Cas où on rencontre un élément à insérer
            format++;
            switch(*format){    //On détermine le type de l'élément
                case 'c':   //Char
                    char actuel = (char) va_arg(args, int);  //On récupère l'élément
                    ecrire(&actuel, 1, 1, stdout);  //On l'écrit
                    nbr_ecrit++;
                    break;
                case 's':   //String
                    char * string = va_arg( args, char * ); //On récupère l'élément
                    while(*string != '\0'){   //On écrit chaque caractère de la chaîne
                        ecrire(string, 1, 1, stdout);   //On l'écrit
                        nbr_ecrit++;    
                        string++;
                    }
                    break;
                case 'd':   //Int
                    int i = va_arg(args, int);  //On récupère l'élément
                    int length = int_to_string(i,buffer);   //On convertit l'entier en chaîne de caractères
                    ecrire(buffer, sizeof(char), length, stdout);   //On l'écrit
                    nbr_ecrit += length;
                    break;
                default:
                    return 1;
            }
        }
        else{
            //Cas où on traite un caractère simple
            ecrire(format, sizeof(char), 1, stdout);
            nbr_ecrit ++;
        }
        format++;
    }

    //On libère les arguments
    va_end(args);
    return nbr_ecrit;
}


int fliref (FICHIER *f, const char *format, ...) {
    
    if(f == NULL || f->buffer == NULL || f->mode != 'L'){   //On vérifie que les entrées sont valides
        return 0;
    }

    //Initialisation des arguments
    va_list args;
    va_start(args, format);

    
    int nbr_lecture = 0;    //Nombre d'éléments lus
    char tmp;   
    int lu = -1;    //Permet de savoir si on a lu un caractère
    int flag = 0;   //Permet de savoir si on a lu un caractère en plus

    while(*format != '\0'){ //On parcourt la chaîne de format
        if(*format == '%'){ //Cas où l'on a un élément à récupérer
            format++;
            switch(*format){    //On détermine le type de l'élément
                case 'c' :  //Char
                    char *c = va_arg(args, char *); //On récupère l'emplacement où l'on doit insérer l'élément
                    if(flag == 1){  //Si l'on a déjà lu le caractère, on l'insère
                        *c = tmp;
                        flag = 0;
                    }
                    else if(lire(&tmp, 1, 1, f) == 1){  //Sinon on le lit
                        nbr_lecture++;
                    }
                    else {  //Si il n'a pas pu être lu, on arrête
                        return nbr_lecture;
                    }
                break;
                case 's' :  //String
                    char *s = va_arg(args, char *); //On récupère l'emplacement où l'on doit insérer l'élément
                    int i = 0;  //Index du tableau
                    lu = -1;
                    if(flag == 1 && tmp != ' ' && tmp != '\n' && tmp != '\0' && tmp != '\t'){  //Si l'on a déjà lu un caractère, on l'insère
                        s[i] = tmp;
                        i++;
                    }
                    while((lu=lire(&tmp, 1, 1, f)) == 1 && tmp != ' ' && tmp != '\n' && tmp != '\0' && tmp != '\t'){    //On lit les caractères jusqu'à un séparateur
                        s[i] = tmp;
                        i++;
                    }
                    s[i] = '\0';    //On ajoute le caractère de fin de chaîne
                    nbr_lecture++;
                    if(lu == 0 || *format == '\0'){ //Si on est à la fin du fichier ou de la chaîne, on arrête
                        return nbr_lecture;
                    }
                    flag = 1;   //On indique que l'on a lu un caractère en plus
                break;
                case 'd' :  //Int
                    int *integer = va_arg(args, int *);  //On récupère l'emplacement où l'on doit insérer l'élément
                    char buffer[11];    //Buffer pour transformer les entiers, la taille est de 11 car un entier ne dépasse pas 10 chiffres et il faut un caractère de fin de chaîne
                    lu = -1;
                    int ctr = 0;
                    if(flag == 1 && tmp <= '9' && tmp >= '0'){  //Si l'on a déjà lu un caractère et que c'est un chiffre, on l'insère
                        buffer[ctr] = tmp;
                        ctr++;
                    }
                    while((lu = lire(&tmp, 1, 1, f)) == 1 &&  tmp <= '9' && tmp >= '0'){    //On lit les chiffres
                        buffer[ctr] = tmp;
                        ctr++;
                    }
                    buffer[ctr] = '\0'; //On ajoute le caractère de fin de chaîne
                    *integer = atoi(buffer);    //On transforme la chaîne en entier
                    nbr_lecture++;
                    if(lu == 0 || *format == '\0'){ //Si on est à la fin du fichier ou de la chaîne, on arrête
                        return nbr_lecture;
                    }
                    flag = 1;   //On indique que l'on a lu un caractère en plus
                    
                break;
                default:
                    return 1;
                
            }
        }
        else{
            //Cas où l'on traite un caractère simple
            if(flag == 1){  //Si on a déjà lu un caractère
                flag = 0;
            }
            else{   //Sinon, on lit le caractère
                lire(&tmp, 1, 1, f);
            }
            if(tmp != *format){ //On vérifie que le caractère lu est bien celui attendu
                return nbr_lecture;
            }
        }
        
        format++;
    }

    //On libère les arguments
    va_end(args);
    return nbr_lecture;
    
}

int fermer(FICHIER *f){
    //On vérifie que l'entrée est valide
    if(f == NULL){
        return 0;
    }
    if(f->mode == 'E'){
        vider(f);   //On vide le buffer si le mode est 'E' (écriture)
    }
    //On vérifie que le buffer est bien alloué
    if(f->buffer != NULL){
        free(f->buffer);    //Libération du buffer
    }
    //Libération de la structure FICHIER
    free(f);

    return 0;
}


int int_to_string(int number, char* tab){
    int p = number; 
    int size = 0; 

    if(number < 10){ //Cas où le nombre est inférieur à 10
        tab[0] = 48 + number;
        size += 1;
        return size; //On retourne la taille de la chaîne
    }
    while(p != 0){ //On calcule la taille de la chaîne
        size += 1;
        p = p/10;
    }
    p = number; //On réinitialise p
    for(int i = size; i>0;i--){ //On remplit le tableau
        tab[i-1]= (p%10) + 48; //On ajoute 48 pour obtenir le caractère ASCII correspondant
        p = p /10; //On passe au chiffre suivant
    }
    return size; //On retourne la taille de la chaîne
}