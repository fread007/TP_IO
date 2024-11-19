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
    char c[50];
    int i;
    //int j =0;

    FICHIER *f1 = ouvrir("TEST/test.txt", 'L');
    if (f1 == NULL){
        exit (-1);
    }


    fliref(f1,"j'ai %s %d.", c,&i);
    ecriref("j'ai %s%d UwU.\n", c,i);
    
    fermer (f1);
    fermer (stdout);

	return 0;
}