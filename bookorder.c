#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv){

	FILE *databasefile;
	databasefile = fopen(argv[1], "r"); //read in user database
	
	if(databasefile == NULL){
		fprintf(stderr, "Invalid user database file.\n");
		return -1;
	}

	//get number of lines in the file for the hash table
	
	int c;
	int numlines;
	while((c = fgetc(databasefile)) != EOF){
		if(c == '\n'){
			numlines++;	
		}
	}

	fclose(databasefile);

	return 0;
}
