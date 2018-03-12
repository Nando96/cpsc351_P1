
#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "msg.h"    /* For the message struct */
#include <fstream> ////for making files

/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void* sharedMemPtr;

/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory 
 * @param msqid - the id of the shared memory
 */

void init(int& shmid, int& msqid, void*& sharedMemPtr)
{
	/* TODO: 
        1. Create a file called keyfile.txt containing string "Hello world" (you may do
 		    so manually or from the code).
     
	    2. Use ftok("keyfile.txt", 'a') in order to generate the key.
		3. Use the key in the TODO's below. Use the same key for the queue
		    and the shared memory segment. This also serves to illustrate the difference
		    between the key and the id used in message queues and shared memory. The id
		    for any System V objest (i.e. message queues, shared memory, and sempahores) 
		    is unique system-wide among all SYstem V objects. Two objects, on the other hand,
		    may have the same key.
	 */
    //1
    ofstream filegen;
    filegen.open("keyfile.txt");
    filegen << "Hello world";
    filegen.close();
    //2
	key_t key = ftok("keyfile.txt", 'a');
    //3
    //use the key to do the following TODOs V
    
	/* TODO: Get the id of the shared memory segment. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE */
    //shmid is the shared memory segment   &&    msqid is the shated memory segment
    shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, IPC_CREAT);
    
    if(shmid == -1){
        perror("shmget: "); //error msg
    }
	/* TODO: Attach to the shared memory */
    sharedMemPtr = shmat(shmid,NULL,0);
    
    if(sharedMemPtr == (void*)-1){
        perror("shmat: "); //error msg
    }
	/* TODO: Attach to the message queue */
    msqid = msgget(key,IPC_CREAT);
    
    if(msqid == -1){
        perror("msgget: ");//error msg
    }
	/* Store the IDs and the pointer to the shared memory region in the corresponding parameters */
    
	
}

/**
 * Performs the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */

void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
	/* TODO: Detach from shared memory */
    int detach_return = shmdt(sharedMemPtr);// detaches shared mem
    if(detach_return == -1){
        perror("shmdt: ");//error msg
    }
    
    }
}

/**
 * The main send function
 * @param fileName - the name of the file
 */
void send(const char* fileName)
{
	/* Open the file for reading */
	FILE* fp = fopen(fileName, "r");
	

	/* A buffer to store message we will send to the receiver. */
	message sndMsg; 
	
	/* A buffer to store message received from the receiver. */
	message rcvMsg;
	
	/* Was the file open? */
	if(!fp)
	{
		perror("fopen");
		exit(-1);
	}
	
	/* Read the whole file */
	while(!feof(fp))
	{
		/* Read at most SHARED_MEMORY_CHUNK_SIZE from the file and store them in shared memory. 
 		 * fread will return how many bytes it has actually read (since the last chunk may be less
 		 * than SHARED_MEMORY_CHUNK_SIZE).
 		 */
		if((sndMsg.size = fread(sharedMemPtr, sizeof(char), SHARED_MEMORY_CHUNK_SIZE, fp)) < 0)
		{
			perror("fread");
			exit(-1);
		}
		
			
		/* TODO: Send a message to the receiver telling him that the data is ready 
 		 * (message of type SENDER_DATA_TYPE) 
 		 */
        //set message type with ".mtype ="
        //int msgsnd ( int msqid, struct msgbuf *msgp, int msgsz, int msgflg );
        //msgflag can be set to zero or IPC_NOWAIT
        sndMsg.mtype = SENDER_DATA_TYPE;
        int sndMsg_return = msgsnd(msqid,&sndMsg,sizeof(message),0);
        if(sndMsg_return == -1){
            perror("msgsnd: ");
        }
		
		/* TODO: Wait until the receiver sends us a message of type RECV_DONE_TYPE telling us 
 		 * that he finished saving the memory chunk. 
         */
        //msgrcv(int msqid, void *msgp, size_t msgsz, long int msgtyp,int msgflg);
        int msgrcv_return = msgrcv(msqid,&rcvMsg,sizeof(message),RECV_DONE_TYPE,0);
        if(msgrcv_return == -1){
            perror("msgrcv: ");
        }
	}
	

	/** TODO: once we are out of the above loop, we have finished sending the file.
 	  * Lets tell the receiver that we have nothing more to send. We will do this by
 	  * sending a message of type SENDER_DATA_TYPE with size field set to 0. 	
	  */
        msgsnd(msqid,&sndMsg,0,)
		
	/* Close the file */
	fclose(fp);
	
}


int main(int argc, char** argv)
{
	
	/* Check the command line arguments */
	if(argc < 2)
	{
		fprintf(stderr, "USAGE: %s <FILE NAME>\n", argv[0]);
		exit(-1);
	}
		
	/* Connect to shared memory and the message queue */
	init(shmid, msqid, sharedMemPtr);
	
	/* Send the file */
	send(argv[1]);
	
	/* Cleanup */
	cleanUp(shmid, msqid, sharedMemPtr);
		
	return 0;
}
