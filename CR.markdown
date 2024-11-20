 # Compte Rendu ENTREE-SORTIE

## introduction

Dans ce TP nous avos du recoder les fonctions principales d'entrée et sortie. Nous avons du Coder les fonctions suivante :
1. `FICHIER *ouvrir(const char *nom, char mode)`
2. `int fermer(FICHIER*f)`
3. `int lire(void *p, unsigned int taille, unsigned int nbelem, FICHIER *f)`
4. `int ecrire(const void *p, unsigned int taille, unsigned int nbelem, FICHIER *f)`
5. `int vider(FICHIER *f);`
6. `int fecriref (FICHIER *f, const char *format, ...)`
7. `int ecriref (const char *format, ...)`
8. `int fliref (FICHIER *f, const char *format, ...)`


## Structure FICHIER

Ont utilise une structure pour stocker les informations sur le fichier ouvert :

```c
typedef struct _ES_FICHIER{
    int descipteur; //descripteur du fichier
    char mode;      // mode d'ouverture 'R' ou 'W'
    char *buffer;   //buffer d'écriture ou de lecture
    int index;      //index de l'octee a lir
    int nbrOctets;  //nombre d'octets non lu dans le buffer
} FICHIER;
```

* descipteur : est le descipteur du fichier ouvert
* mode : est le mode d'ouverture du fichier 'R' pour lecture et 'W' pour écriture
* buffer : est le tableau de char qui represente les octee lu ou ecris
* index : est l'index de l'octet a lire ou ecrire
* nbrOctets : est le nombre d'octets dans le buffer

## Fonction ouvrir

`FICHIER *ouvrir(const char *nom, char mode)`

La fonction ouvrir permet d'ouvrir un fichier en ecriture ou en lecture ,on ne permet pas d'ouvrire en lecture ET ecriture en meme temps.
Dans cette fonction on alloue de la memoire pour notre structure FICHIER et pour le buffer. Ensuite on ouvre le fichier a l'aide de la fonction `open` et on verifie si l'ouverture c'est bien passé.

## Fonction fermer

`int fermer(FICHIER*f)`

La fonction fermer permet de fermer un fichier ouvert aprer avoir vider sont buffer. On libere la memoire alloué pour le buffer et pour la structure FICHIER. Ensuite on ferme le fichier a l'aide de la fonction `close`.

## Fonction lire

`int lire(void *p, unsigned int taille, unsigned int nbelem, FICHIER *f)`

La fonction lire simule la fonction `read` de la librairie standard en y ajoutant un buffer pour eviter un appel systeme a chaque lecture. On verifie tout d'abord si le buffer est vide ou si on a deja lu tout les octets du buffer. Si c'est le cas ont copie la fin du buffer(la partie non lu) dans le debut du buffer et on lit le restent dans le fichier a l'aide de la fonction `read`. Puis on copie le nombre d'octets que l'on veut lire(taille * nbelem) a l'adresse du pointeur donné en parametre.

La fonction renvoie le nombre d'éléments lus.

## Fonction ecrire

`int ecrire(const void *p, unsigned int taille, unsigned int nbelem, FICHIER *f)`

La fonction ecrire simule la fonction `write` de la librairie standard en y ajoutant un buffer pour eviter un appel systeme a chaque ecriture. On verifie tout d'abord si le buffer a assez de place pour ecrire les octets donnée en parametre. Si c'est le cas on copie les octets dans le buffer. Sinon on ecris le buffer dans le fichier. Puis on copie le nombre d'octets que l'on veut ecrire(taille * nbelem) dans le buffer, les donné a ecrire sont a l'adresse du pointeur donné en parametre.

## Fonction vider 

`int vider(FICHIER *f);`

La fonction vider simule la fonction `fflush` de la librairie standard. Elle permet de vider le buffer du fichier associé au descripteur de fichier donnée en argument.

## Fonction fecriref

`int fecriref (FICHIER *f, const char *format, ...)`

La fonction fecriref simule la fonction `fprintf` de la librairie standard. Elle permet d'ecrire dans un fichier ouvert en mode écriture. Elle prend en parametre un pointeur sur un fichier ouvert et une chaine de caractère formatée, ainsi qu'un nombre variable d'arguments. Tout les arguments sont de type int,char ou String. La chaine format est parcourue caractère par caractère et si on trouve un % on regarde le caractère suivant pour savoir quel type de variable on doit ecrire. Ensuite on recuper la valeur dans la variable suivante. On utilise la fonction ecrire pour ecrire les valeurs dans le fichier. 
Dans le cas on ont veut ecire un String ( %s ) ont traite caractère par caractère le pour sasure que la String est ecrite en entier.
Dans le cas ou on veut ecrire un entier ( %d ) on utilise la fonction `int_to_string` pour convertir l'entier en String.

La fonction renvoie le nombre d'element ecrit.

## Fonction ecriref

`int ecriref (const char *format, ...)`

La fonction ecriref simule la fonction `printf` de la librairie standard. La fonction fait la même choses que fecriref mais sur la  sortie standard.

## Fonction fliref

`int fliref (FICHIER *f, const char *format, ...)`

La fonction fliref simule la fonction `fscanf` de la librairie standard. Elle permet de lire dans un fichier ouvert en mode lecture. Elle prend en parametre un pointeur sur un fichier ouvert et une chaine de caractère formatée, ainsi qu'un nombre variable d'arguments. Tout les arguments sont des pointeurs sur des type int,char ou String. La chaine format est parcourue caractère par caractère et si on trouve un % on regarde le caractère suivant pour savoir quel type de variable on doit lire. Ensuite on lit la valeur dans le fichier. 
Dans le cas on ont veut lire un String ( %s ) ont lit caractère par caractère le jusqu'a trouver un separateur (espace,tabulation,retour a la ligne,fin).
Dans le cas ou on veut lire un entier ( %d ) on met chaque digit dans un tableau puis on converti le tableau en entier grace a la fonction `atoi`.

La fonction renvoie le nombre d'element lu.


