#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include "msg.h"

/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void *sharedMemPtr;

/* The name of the received file */
const char recvFileName[] = "recvfile";


/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory 
 * @param msqid - the id of the shared memory
 * @param sharedMemPtr - the pointer to the shared memory
 */

void init(int& shmid, int& msqid, void*& sharedMemPtr)
{
	
	/* TODO: 1. Create a file called keyfile.txt containing string "Hello world" (you may do
 		    so manually or from the code).
	         2. Use ftok("keyfile.txt", 'a') in order to generate the key.
		 3. Use the key in the TODO's below. Use the same key for the queue
		    and the shared memory segment. This also serves to illustrate the difference
		    between the key and the id used in message queues and shared memory. The id
		    for any System V object (i.e. message queues, shared memory, and sempahores) 
		    is unique system-wide among all System V objects. Two objects, on the other hand,
		    may have the same key.
	 */
	

	
	/* TODO: Allocate a piece of shared memory. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE. */
	/* TODO: Attach to the shared memory */
	/* TODO: Create a message queue */
	/* Store the IDs and the pointer to the shared memory region in the corresponding parameters */
	
	//1 make keyfile.txt
printf("making key\n");
	std::ofstream outfile ("keyfile.txt");
	outfile << "Hello world";
	outfile.close();

	//2 generate key
	key_t key = ftok("keyfile.txt", 'a');
	if (key < 0){
	perror("key");
	exit(-1);
	}
	//3 Get id of the shared memory segment
	shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, 0644|IPC_CREAT);
	if (shmid < 0){
	perror("shmid");
	exit(-1);
	}
	//Attach to the shared memory
	sharedMemPtr = shmat(shmid,NULL,0);

	if(sharedMemPtr < (char*)0){
	perror("sharedMemPtr");
	exit(-1);
	}

	//Attach to the message queue
	msqid = msgget(key, 0644|IPC_CREAT);

	if(msqid < 0){
	perror("msqid");
	exit(-1);
	}
	/* Store the IDs and the pointer to the shared memory region in the corresponding parameters */

	
}


/**
 * The main loop
 */
void mainLoop(){

	/* The size of the mesage */
	int msgSize = 0;
	
	/* Open the file for writing */
	FILE* fp = fopen(recvFileName, "w");
		
	/* Error checks */
	if(!fp)
	{
		perror("fopen");	
		exit(-1);
	}
printf("waiting for sender\n");		
    /* TODO: Receive the message and get the message size. The message will 
     * contain regular information. The message will be of SENDER_DATA_TYPE
     * (the macro SENDER_DATA_TYPE is defined in msg.h).  If the size field
     * of the message is not 0, then we copy that many bytes from the shared
     * memory region to the file. Otherwise, if 0, then we close the file and
     * exit.
     *
     * NOTE: the received file will always be saved into the file called
     * "recvfile"
     */

	/* Keep receiving until the sender set the size to 0, indicating that
 	 * there is no more data to send
 	 */	
	message tempMess;
	if(msgrcv(msqid, &tempMess, sizeof(tempMess) , SENDER_DATA_TYPE,0)<0){
		perror("msgrcv");	
	}
	msgSize = tempMess.size;
printf("entering msgSize loop\n");
	while(msgSize != 0)
	{	
		/* If the sender is not telling us that we are done, then get to work */
		if(msgSize != 0)
		{
			/* Save the shared memory to file */
			if(fwrite(sharedMemPtr, sizeof(char), msgSize, fp) < 0)
			{
				perror("fwrite");
			}
			
			/* TODO: Tell the sender that we are ready for the next file chunk. 
 			 * I.e. send a message of type RECV_DONE_TYPE (the value of size field
 			 * does not matter in this case). 
 			 */
			tempMess.mtype = RECV_DONE_TYPE;//set message to RCV_DONE_TYPE
printf("we are rdy for next chunk\n");
			if(msgsnd(msqid,&tempMess, 0, 0)<0){
				perror("msgsnd");
			}
			if(msgrcv(msqid,&tempMess,sizeof(tempMess) , SENDER_DATA_TYPE,0)<0){
				perror("msgrcv");
			}
			msgSize = tempMess.size;//get current msgSize to prevent inf loop
printf("got new msg\n");
		}
		/* We are done */
		else
		{
			/* Close the file */
			fclose(fp);
		}
	}
printf("loop is done\n");
}



/**
 * Perfoms the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */

void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
	/* TODO: Detach from shared memory */
	if(shmdt(sharedMemPtr)<0){
		perror("shmdt");
	}
	/* TODO: Deallocate the shared memory chunk */
	if(shmctl(shmid, IPC_RMID, NULL)<0){
		perror("shmctl");
	}
	/* TODO: Deallocate the message queue */
	if(msgctl(msqid, IPC_RMID, NULL)<0){
		perror("msgctl");
	}
}

/**
 * Handles the exit signal
 * @param signal - the signal type
 */

void ctrlCSignal(int signal)
{
	/* Free system V resources */
	cleanUp(shmid, msqid, sharedMemPtr);
}

int main(int argc, char** argv)
{
printf("start recv\n");
	/* TODO: Install a singnal handler (see signaldemo.cpp sample file).
 	 * In a case user presses Ctrl-c your program should delete message
 	 * queues and shared memory before exiting. You may add the cleaning functionality
 	 * in ctrlCSignal().
 	 */
	signal(SIGINT, ctrlCSignal);
printf("\tinit\n");				
	/* Initialize */
	init(shmid, msqid, sharedMemPtr);
printf("\tmainloop\n");	
	/* Go to the main loop */
	mainLoop();

	/** TODO: Detach from shared memory segment, and deallocate shared memory and message queue (i.e. call cleanup) **/
printf("\tcleaning up\n");
	cleanUp(shmid, msqid, sharedMemPtr);
printf("\tDone\n");	
	return 0;
}
