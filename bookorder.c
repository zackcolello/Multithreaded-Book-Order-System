#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bookorder.h"
#include "queue.h"
#include <pthread.h>


struct producerargs{

	FILE* orderfile;
	struct queue* catQ;

};


//Producer thread has access to the orders file, and all queues. 
//Producer creates ordernode, and adds to category queue.
void* producer(void* arguments){

	
	struct producerargs *args = (struct producerargs*)arguments;

	char* word = malloc(1000);

	strcpy(word, args->catQ[0].category);

}

int main(int argc, char** argv){

	FILE *databasefile;
	databasefile = fopen(argv[1], "r"); //read in user database
	
	if(databasefile == NULL){
		fprintf(stderr, "Invalid user database file.\n");
		return -1;
	}

	//get number of lines in the database file for the hash table
	
	int c;
	int numlines;
	while((c = fgetc(databasefile)) != EOF){
		if(c == '\n'){
			numlines++;	
		}
	}

	//create hash table customerDatabase

	fseek(databasefile, 0, SEEK_SET);
	
	struct customerDatabase* DB = (struct customerDatabase*)malloc(sizeof(struct customerDatabase)*numlines);
	char* token;
	char buffer[1000];
	int i = 0;


	while(fgets(buffer, 1000, databasefile)){

		
		token = strtok(buffer, "\"");

		//add database name
		(DB[i]).name = (char*)malloc(strlen(token));				
		strcpy(DB[i].name, buffer+1);


		//add database id
		token = strtok(NULL, "|\"");
		(DB[i]).id = atoi(token);		


		//add database balance
		token = strtok(NULL, "|\"");
		(DB[i]).balance = atof(token);		

		//add database address
		token = strtok(NULL, "|\"");
		(DB[i]).address = (char*)malloc(strlen(token));				
		strcpy(DB[i].address, token);


		//add database state
		token = strtok(NULL, "|\"");
		(DB[i]).state = (char*)malloc(strlen(token));				
		strcpy(DB[i].state, token);


		//add database zip
		token = strtok(NULL, "|\"");
		(DB[i]).zip = (char*)malloc(strlen(token));				
		strcpy(DB[i].zip, token);
	
		DB[i].success = NULL;
		DB[i].failure = NULL;

		i++;
	}	 	
	
	fclose(databasefile);	

	//read in category file,  make queue
	FILE *categoryfile;
	categoryfile = fopen(argv[3], "r");
	
	if(categoryfile == NULL){
		fprintf(stderr, "Invalid category file.\n");
		return -1;
	}

	//get number of lines in category file

	i = 0;	
	numlines = 0;

	while((c = fgetc(categoryfile)) != EOF){
		if(c == '\n'){
			numlines++;
		}
	}	

	fseek(categoryfile, 0, SEEK_SET);


	//create queue 
	struct queue* catQ = (struct queue*)malloc(sizeof(struct queue)*numlines); 

	while(fgets(buffer, 1000, categoryfile)){

		token = strtok(buffer, "\n");		

		catQ[i].category = (char*)malloc(strlen(buffer));
		strcpy(catQ[i].category, buffer);	
		printf("catQ is %s\n", catQ[i].category);	
		i++;
	}


	//make producer thread and struct with arguments to give to producer function
	
	pthread_t thread; 
	struct producerargs* pargs;
	pargs = (struct producerargs*)malloc(sizeof(struct producerargs));
	pargs->catQ = catQ;


	FILE* orderfile = fopen(argv[2], "r");

	if(orderfile == NULL){
		fprintf(stderr, "Invalid order file.\n");
		return -1;
	}

	pargs->orderfile = orderfile;

	int prodint = pthread_create(&thread, NULL, producer, (void*)(pargs));

	pthread_join(thread, NULL);


//	return 0;
}
