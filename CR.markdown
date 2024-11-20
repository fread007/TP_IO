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

On utilise une structure pour stocker les informations sur le fichier ouvert :

```c
typedef struct _ES_FICHIER{
    int descripteur; //descripteur du fichier
    char mode;      // mode d'ouverture 'R' ou 'W'
    char *buffer;   //buffer d'écriture ou de lecture
    int index;      //index de l'octets a lire
    int nbrOctets;  //nombre d'octets non lu dans le buffer
} FICHIER;
```

* descripteur : est le descripteur du fichier ouvert
* mode : est le mode d'ouverture du fichier 'R' pour lecture et 'W' pour écriture
* buffer : est le tableau de char qui represente les octee lu ou ecris
* index : est l'index de l'octets a lire ou ecrire
* nbrOctets : est le nombre d'octets dans le buffer

## Fonction ouvrir

`FICHIER *ouvrir(const char *nom, char mode)`

La fonction ouvrir permet d'ouvrir un fichier en écriture ou en lecture. On ne permet pas d'ouvrir un fichier en lecture et écriture en même temps.
Dans cette fonction, on alloue de la mémoire pour notre structure FICHIER et pour le buffer. Ensuite, on ouvre le fichier à l'aide de la fonction `open` et on vérifie si l'ouverture s'est bien passée.

## Fonction fermer

`int fermer(FICHIER*f)`

La fonction fermer permet de fermer un fichier ouvert après avoir vidé son buffer. On libère la mémoire allouée pour le buffer et pour la structure FICHIER. Ensuite, on ferme le fichier à l'aide de la fonction `close`.

## Fonction lire

`int lire(void *p, unsigned int taille, unsigned int nbelem, FICHIER *f)`

La fonction lire simule la fonction `read` de la librairie standard en y ajoutant un buffer pour éviter un appel système à chaque lecture. On vérifie tout d'abord si le buffer est vide ou si on a déjà lu tous les octets du buffer. Si c'est le cas, on copie la fin du buffer (la partie non lue) dans le début du buffer et on lit le reste dans le fichier à l'aide de la fonction `read`. Puis, on copie le nombre d'octets que l'on veut lire (taille * nbelem) à l'adresse du pointeur donné en paramètre.

La fonction renvoie le nombre d'éléments lus.

## Fonction ecrire

`int ecrire(const void *p, unsigned int taille, unsigned int nbelem, FICHIER *f)`

La fonction ecrire simule la fonction `write` de la librairie standard en y ajoutant un buffer pour éviter un appel système à chaque écriture. On vérifie tout d'abord si le buffer a assez de place pour écrire les octets donnés en paramètre. Si c'est le cas, on copie les octets dans le buffer. Sinon, on écrit le buffer dans le fichier. Puis, on copie le nombre d'octets que l'on veut écrire (taille * nbelem) dans le buffer. Les données à écrire sont à l'adresse du pointeur donné en paramètre.

## Fonction vider 

`int vider(FICHIER *f);`

La fonction vider simule la fonction `fflush` de la librairie standard. Elle permet de vider le buffer du fichier associé au descripteur de fichier donnée en argument.

## Fonction fecriref

`int fecriref (FICHIER *f, const char *format, ...)`

La fonction fecriref simule la fonction `fprintf` de la librairie standard. Elle permet d'écrire dans un fichier ouvert en mode écriture. Elle prend en paramètre un pointeur sur un fichier ouvert, une chaîne de caractères formatée, ainsi qu'un nombre variable d'arguments. Tous les arguments sont de type int, char ou String. La chaîne de format est parcourue caractère par caractère et, si on trouve un %, on regarde le caractère suivant pour savoir quel type de variable on doit écrire. Ensuite, on récupère la valeur dans la variable suivante. On utilise la fonction ecrire pour écrire les valeurs dans le fichier.
Dans le cas où l'on veut écrire un String (%s), on traite caractère par caractère pour s'assurer que la chaîne est écrite en entier.
Dans le cas où l'on veut écrire un entier (%d), on utilise la fonction  `int_to_string` pour convertir l'entier en String.

La fonction renvoie le nombre d'éléments écrits.

## Fonction ecriref

`int ecriref (const char *format, ...)`

La fonction ecriref simule la fonction `printf` de la librairie standard. La fonction fait la même choses que fecriref mais sur la  sortie standard.

## Fonction fliref

`int fliref (FICHIER *f, const char *format, ...)`

La fonction fliref simule la fonction `fscanf` de la librairie standard. Elle permet de lire dans un fichier ouvert en mode lecture. Elle prend en paramètre un pointeur sur un fichier ouvert, une chaîne de caractères formatée, ainsi qu'un nombre variable d'arguments. Tous les arguments sont des pointeurs sur des types int, char ou String. La chaîne de format est parcourue caractère par caractère et, si on trouve un %, on regarde le caractère suivant pour savoir quel type de variable on doit lire. Ensuite, on lit la valeur dans le fichier.
Dans le cas où l'on veut lire un String (%s), on lit caractère par caractère jusqu'à trouver un séparateur (espace, tabulation, retour à la ligne, fin).
Dans le cas où l'on veut lire un entier (%d), on met chaque chiffre dans un tableau puis on convertit le tableau en entier grâce à la fonction `atoi`.

La fonction renvoie le nombre d'éléments lus.


