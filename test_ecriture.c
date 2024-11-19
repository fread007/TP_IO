#include "stdes.h"
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 

int main(int argc, char *argv[])
{
	char s[5] = "hello";
    char vide[50];

    FICHIER *f1 = ouvrir("TEST/test.txt", 'E');

    if (f1 == NULL){
        exit (-1);
    }

    fecriref(f1,"Mon char : %c, Mon int : %d , Ma string : %s\n", 'c' , 42,s);
    ecriref("Mon char : %c, Mon int : %d , Ma string : %s\n", 'c' , 42,s);
    ecriref("HOHOHOOHO\n");
    ecriref("%s\n",vide);
    ecriref("%c\n",60);
    ecriref("%d\n",'c');
        
    vider(stdout);
    fermer (f1);

	return 0;
}