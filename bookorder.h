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
