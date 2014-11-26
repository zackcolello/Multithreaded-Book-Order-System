struct customerDatabase {

	char* name;
	int id;
	double balance;
	char* address;
	char* state;
	char* zip;

	struct queue *success;
	struct queue *failure;

};

struct ordernode {

	char* title;
	double price;
	double currentBalance;
	struct ordernode *next;	
	char* category;
	int id;

};

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

void* producer(void* arguments);
void* consumer(void* arguments);

