
#define SENDER_DATA_TYPE 1
#define RECV_DONE_TYPE 2

struct message
{
	long mtype;
	int size;	
};
