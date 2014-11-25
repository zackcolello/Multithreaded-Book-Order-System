#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bookorder.h"
#include "queue.h"
#include <pthread.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t orders = PTHREAD_COND_INITIALIZER;
pthread_cond_t space = PTHREAD_COND_INITIALIZER;

struct producerargs{

	FILE* orderfile;
	struct queue* catQ;
	int Qsize; //holds how many queues are in catQ

};

struct consumerargs{

	struct customerDatabase* DB;		
	struct queue* q;
	int DBsize;

};

//Producer thread has access to the orders file, and all queues. 
//Producer creates ordernode, and adds to category queue.
void* producer(void* arguments){

	//make void argument back into producerargs struct	
	struct producerargs *args = (struct producerargs*)arguments;

	//read file to create ordernodes for each line.
	//Traverse database to find matching ID for that order, and attempt to place order,
	//adding ordernode to either success or failure queue.
	FILE* ofile = args->orderfile;
	char buffer[1000];

	struct ordernode* new;
	char* token;



	while(fgets(buffer, 1000, ofile)){	

		
		new = (struct ordernode*)malloc(sizeof(struct ordernode));
		
		//add title to order node
		token = strtok(buffer, "\"");
		new->title = (char*)malloc(strlen(token)+1);
		strcpy(new->title, token);		

		//add price to order node
		token = strtok(NULL, "|\"");
		new->price = atof(token);

		//add id to order node
		token = strtok(NULL, "|\"");
		new->id = atoi(token);

		//add category to order node
		token = strtok(NULL, "|\"\n");
		new->category = (char*)malloc(strlen(token)+1);
		strcpy(new->category, token);

		//put orders in queue
		
		int i = 0;
		while(i < args->Qsize){
	
			if(strcmp(args->catQ[i].category,new->category) == 0){

				pthread_mutex_lock (&lock);
				while(args->catQ[i].count >= 5){
					pthread_cond_signal(&orders);
					printf("PRODUCER waits for space in %s\n",args->catQ[i].category);
					pthread_cond_wait(&space, &lock);

					}
				
				printf("PRODUCER adds %s to %s\n", new->title,args->catQ[i].category);
				enqueue(&(args->catQ[i]), new);
				pthread_cond_signal(&orders);
				pthread_mutex_unlock(&lock);
			}

		
			i++;
		}

		//error, did not find category
		if(i > args->Qsize){
			fprintf(stderr, "Category %s does not exist.\n", new->category);
		}

	
	}	
	fclose(ofile);
	pthread_mutex_lock(&lock);
	printf("PRODUCER CLOSES QUEUES\n");
	int k=0;
	while(k<args->Qsize){
		
		args->catQ[k].open=0;
		k++;
	}
	pthread_cond_broadcast(&orders);
	pthread_mutex_unlock(&lock);
}


void* consumer(void* arguments){


	struct consumerargs *args = (struct consumerargs*)arguments;
	struct ordernode *order;
	

	while(args->q->open==1|| args->q->count>0){

		pthread_mutex_lock(&lock);

		
		while(args->q->count== 0 && args->q->open==1){

			pthread_cond_signal(&space);
			printf("CONSUMER waits for order in %s\n",args->q->category);
			pthread_cond_wait(&orders,&lock);
		
			printf("CONSUMER resumes at %s\n", args->q->category);
			if(args->q->open==0){
				printf("CONSUMER QUEUE %s has closed\n",args->q->category);
				break;
			}
		}
		
		order = (struct ordernode *)dequeue((struct queue*)args->q);

		if(order!= NULL){

		//WRITE CODE HERE FOR DB STUFF
	
			//find matching customer id in database
			int i;
			for(i=0; i < args->DBsize; i++){ 

				//found matching id
				if(args->DB[i].id == order->id){


					//success!
					if(args->DB[i].balance >=  order->price){
	
	
						order->currentBalance = args->DB[i].balance - order->price;
						args->DB[i].balance = order->currentBalance;
						enqueue(args->DB[i].success, order);		


					//not enough money
					}else{
						enqueue(args->DB[i].failure, order);

					}

				}
			}
			




		//printf("%s\n\t%s\n", order->category, order->title);	
		}
	
		pthread_cond_signal(&space);
		pthread_mutex_unlock(&lock);

	
	}
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
	int DBnumlines=0;
	while((c = fgetc(databasefile)) != EOF){
		if(c == '\n'|| c==EOF){
		
			DBnumlines++;
		}
	}
	//create array of structs called customerDatabase

	fseek(databasefile, 0, SEEK_SET);
	
	struct customerDatabase* DB = (struct customerDatabase*)malloc(sizeof(struct customerDatabase)*DBnumlines);
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
	
		DB[i].success = (struct queue*)malloc(sizeof(struct queue));
		DB[i].failure = (struct queue*)malloc(sizeof(struct queue));
		DB[i].success->rear = NULL;
		DB[i].failure->rear = NULL;


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
	int numlines = 0;

	while((c = fgetc(categoryfile)) != EOF){
		if(c == '\n'){
			numlines++;
		}
	}	

	fseek(categoryfile, 0, SEEK_SET);


	//create queue array catQ that holds a queue for each category 
	struct queue* catQ = (struct queue*)malloc(sizeof(struct queue)*numlines); 

	while(fgets(buffer, 1000, categoryfile)){

		token = strtok(buffer, "\n");		

		catQ[i].category = (char*)malloc(strlen(buffer));
		strcpy(catQ[i].category, buffer);	
		catQ[i].rear = NULL;
		catQ[i].open=1;

		i++;
	}
	fclose(categoryfile);

	//make producer thread and struct with arguments to give to producer function
	
	pthread_t thread;

	struct producerargs* pargs;
	pargs = (struct producerargs*)malloc(sizeof(struct producerargs));
	pargs->catQ = catQ;
	pargs->Qsize = numlines;

	FILE* orderfile = fopen(argv[2], "r");

	if(orderfile == NULL){
		fprintf(stderr, "Invalid order file.\n");
		return -1;
	}

	pargs->orderfile = orderfile;

	int prodint = pthread_create(&thread, NULL, producer, (void*)(pargs));
	
	//make sure thread is zero
	if(prodint){
		fprintf(stderr, "Error: Int from pthread_create is: %d\n", prodint);
		exit(-1);
	}

	
	//create a consumer thread for each category
	pthread_t consThreads[numlines];

	struct consumerargs* cargs;

	
	int index, consint;
	
	cargs = (struct consumerargs*)malloc(sizeof(struct consumerargs)*numlines);

	
	for(index = 0; index < numlines; index++){ 
		
		cargs[index].q = &catQ[index]; 
		cargs[index].DB = DB;
		cargs[index].DBsize = DBnumlines;

		if(consint = pthread_create(&consThreads[index], NULL, consumer, (void*)(&(cargs[index])))){
			fprintf(stderr, "Error: Int from pthread_create is: %d\n", consint);
			exit(-1);
		}


	}

	pthread_join(thread, NULL);


	for(index = 0; index < numlines; index++){
		pthread_join(consThreads[index], NULL);
	}
	
	FILE *reportfile;
	reportfile=fopen("finalreport.txt","w+");
	struct ordernode *reprint;	
	int x;
	for(x = 0; x<DBnumlines; x++){
	//	printf("=== BEGIN CUSTOMER INFO===\n");
		fprintf(reportfile,"=== BEGIN CUSTOMER INFO===\n###BALENCE###\nCustumer name: %s\nCustumer ID number: %d\nRemaining credit balance after all purchases (dollar amount): %.02f\n###SUCCESSFUL ORDERS###\n", DB[x].name, DB[x].id, DB[x].balance);
		
		int y;

		if(DB[x].success->rear){
			reprint = DB[x].success->rear->next;
				for(y=0;y<DB[x].success->count;y++){
					fprintf(reportfile,"\"%s\"|%.02f|%.02f\n",reprint->title,reprint->price,reprint->currentBalance);
			
					reprint=reprint->next;
				}
			}


		fprintf(reportfile,"###REJECTED ORDERS###\n");

		if(DB[x].failure->rear){
			reprint = DB[x].failure->rear->next;
			for(y=0;y<DB[x].failure->count;y++){
				fprintf(reportfile,"\"%s\"|%.02f\n",reprint->title,reprint->price);
				reprint=reprint->next;
			}
		}
		fprintf(reportfile,"===END CUSTOMER INFO===\n\n");

		destroyQ(DB[x].success);
		destroyQ(DB[x].failure);
		free(DB[x].name);
		free(DB[x].address);
		free(DB[x].state);
		free(DB[x].zip);
		free(DB[x].success);
		free(DB[x].failure);
	//	printf("%s could not afford:\n", DB[x].name);

	//	displayqueue(DB[x].failure);

	}
	free(DB);
	int l;
	for(l=0;l<numlines;l++){
		free(catQ[l].category);;
	}
	free(cargs);
	free(pargs);
	free(catQ);
	fclose(reportfile);
	

	return 0;
}
