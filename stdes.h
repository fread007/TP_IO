#ifndef _STDES_H
#define _STDES_H

struct _ES_FICHIER;
typedef struct _ES_FICHIER{
    int descipteur; //descripteur du fichier
    char mode;      // mode d'ouverture 'R' ou 'W'
    char *buffer;   //buffer d'écriture ou de lecture
    int index;      //index de l'octee a lir
    int nbrOctets;  //nombre d'octets non lu dans le buffer
} FICHIER;

extern FICHIER *stdout;
extern FICHIER *stderr;

/* mode: 'L' = lecture, 'E' = écriture */
FICHIER *ouvrir(const char *nom, char mode);
int fermer(FICHIER*f);
int lire(void *p, unsigned int taille, unsigned int nbelem, FICHIER *f);
int ecrire(const void *p, unsigned int taille, unsigned int nbelem, FICHIER *f);
int vider(FICHIER *f);

int fecriref (FICHIER *f, const char *format, ...);
/* directly in stdout */
int ecriref (const char *format, ...);
int fliref (FICHIER *f, const char *format, ...);

#endif

