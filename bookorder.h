struct customerDatabase {

	char* name;
	int id;
	double balance;
	char* address;
	char* state;
	char* zip;

	struct ordernode *success;
	struct ordernode *failure;

};


struct ordernode {

	char* title;
	double price;
	double currentBalance;
	struct ordernode *next;	
	char* category;
	int id;

};
