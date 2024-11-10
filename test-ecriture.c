#include "stdes.h"
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 

int main(int argc, char *argv[])
{
	char c;


	
	if (argc < 2){
		exit(-1);
	}

    FICHIER *f1 = ouvrir("test.txt", 'E');

    if (f1 == NULL){
        exit (-1);
    }

    int sortie = open("test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    
	for(int i = 1; i < argc; i++){

        ecrire(argv[i], strlen(argv[i]), 1, f1);


        ecrire(" ", 1, 1, f1);
        
    }

    vider(f1);
    fermer (f1);

	return 0;
}
