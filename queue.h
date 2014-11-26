struct queue{

	char* category;
	int count; //total number of nodes in queue
	struct ordernode *rear;
	int open;
};

void displayqueue (struct queue* queue);
struct ordernode* dequeue(struct queue* queue);
void enqueue(struct queue* queue, struct ordernode* newnode);
void destroyQ(struct queue* queue);
