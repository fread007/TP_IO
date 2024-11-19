#include "stdes.h"
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 

int main(int argc, char *argv[])
{

    //char c,d,e;
    //char s[50];
    //char c[50];
    int i;
    int j =0;

    FICHIER *f1 = ouvrir("TEST/test.txt", 'L');
    if (f1 == NULL){
        exit (-1);
    }



    fliref(f1,"j'ai %d%d",&i, &j);
    ecriref("j'ai %d %d ans\n",i,j);
    ecriref("truc %d\n",012);
    fermer (f1);
    fermer (stdout);

	return 0;
}