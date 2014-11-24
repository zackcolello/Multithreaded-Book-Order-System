#include "bookorder.h"
#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

void enqueue(struct queue* queue, struct ordernode* new){




	//check if queue is empty
	if(queue->rear == NULL){
		queue->rear = new;
		queue->count++;
		queue->rear->next = new;
	}else{
		new->next = queue->rear->next;
		queue->rear->next = new;
		queue->rear = new;
		queue->count++;
	}

}

struct ordernode* dequeue(struct queue* queue){

	if(queue->rear == NULL){
		return NULL;	
	}

	struct ordernode *temp = queue->rear->next;

	queue->rear->next = temp->next;
	temp->next = NULL;
	queue->count--;

	return temp;
	

}

void displayqueue (struct queue* queue){

	if(queue->count == 0){
		printf("No items in queue.\n");
		return;
	}

	struct ordernode *temp = queue->rear->next;
	
	do{

		printf("%s ", temp->title);
		
	}while(temp != queue->rear->next);


}
