#include "bookorder.h"
#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

void displayqueue (struct queue* queue){

	if(queue->count == 0){
		printf("No items in queue.\n");
		return;
	}

	struct ordernode *temp = queue->rear->next;
	
	int i;

	for(i = 0; i <queue->count; i++){
		printf("\t%s\n", temp->title);
		temp = temp->next;

	}

}



void enqueue(struct queue* queue, struct ordernode* newnode){



	//check if queue is empty
	if(queue->rear == NULL){
		queue->rear = newnode;
		queue->count++;
		queue->rear->next = newnode;
	}else{
		newnode->next = queue->rear->next;
		queue->rear->next = newnode;
		queue->rear = newnode;
		queue->count++;
	}
	
}

struct ordernode* dequeue(struct queue* queue){


	
	if(queue->rear == NULL){
		return NULL;	
	}

	struct ordernode* temp = queue->rear->next;

	//only one node in queue	
	if(queue->rear == queue->rear->next){
		temp = queue->rear;	
		queue->rear = NULL;
		queue->count = 0;
		return temp;

	}



	queue->rear->next = temp->next;
	temp->next = NULL;
	queue->count--;

	return temp;
	
}


