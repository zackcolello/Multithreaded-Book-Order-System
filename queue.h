struct queue{

	char* category;
	int count; //total number of nodes in queue
	struct ordernode *rear;
	int open;
};

struct ordernode* dequeue(struct queue* queue);
void enqueue(struct queue* queue, struct ordernode* newnode);
