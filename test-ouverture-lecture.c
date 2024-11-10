#include "stdes.h"
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 

int main(int argc, char *argv[])
{
	FICHIER *f1;
	char c;

    int sortie = open("test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
	
	if (argc != 2){
		exit(-1);
	}


	f1 = ouvrir (argv[1], 'L');
	if (f1 == NULL){
		exit (-1);
	}


	while (lire (&c, 1, 1, f1) == 1) {
          write(sortie, &c, 1);
	}

        fermer (f1);

	return 0;
}
