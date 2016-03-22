
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <limits.h>
#include <errno.h>
#include <time.h>


void Gen(int CurrentNum){

    int i=0;
    int r=0;
    //the while loop check if all the n child processes has been created
    while(CurrentNum>0){

        r = rand() % CurrentNum +1; // generate a random number r
	pid_t myArray[r+1];
	//loop r times to create r children 
    	for(i=0; i<r; i++){
		
    		myArray[i] = fork();
		if(i==0){
			if(myArray[0]==0){
				break;
			}
		}
		if(myArray[i] >= 0){ //fork success
			if(myArray[i] ==0){  // useless children
				while(1);	//do nothing
			}
			else{		      
				continue;  //return to for loop; keep generating child processes
			}
		}
		else{
			perror("error occured");
			exit(0);
		}
		
    	}

	 
	CurrentNum -= r;
	if(myArray[0]==0){
		if(CurrentNum==0){
			while(1);
		}
		continue;  //let  the first child keep generating the rest children
	}
	else{		//here is the parent process
		break;	//parent process has finished his job
	}
    } //end while loop

}


int Suspend(pid_t id){
	int retVal = kill(id, SIGSTOP); 
	if(retVal==0){
		printf("suspend successfully\n");
		return;
	}
	if(retVal==-1){
		printf("Suspend failed\n");
	}
}

int Resume(pid_t id){
	int retVal = kill(id, SIGCONT); 
	if(retVal==0){
		printf("resume successfully\n");
		return;
	}
	if(retVal==-1){
		printf("resume failed\n");
	}
}

int Terminate(pid_t id){
	int retVal = kill(id, SIGTERM); 
	if(retVal==0){
		printf("terminated successfully\n");
		return;
	}
	if(retVal==-1){
		printf("terminated failed\n");
	}

}


int main(int argc, char *argv[]) {

    int a =0;
    int command =0;
    int n=0, i=0;
    pid_t suspid = 0, respid=0, terpid=0;
    int count = 0;
    
    time_t t;
    int CurrentNum = 0;
    

// get the number of processes needed
    printf("How many processes you want create and monitor\n");
    scanf("%d" , &n);
    CurrentNum = n;
    //generate random numbers
    //initiate the random number generator 
    srand((unsigned) time(&t));
    //generate the first process

    // cpid = fork();    
    // pidGenerate = getpid();

    Gen(n);   //call processes generator function


    //if user doesn't quit
    while(!a) {
        printf("Please enter a command number ===> 1.List 2.Suspend 3.Resume 4.Terminate 5.Exit...\n");
        scanf("%d", &command);
        //if user types "Exit"
        if (command == 5) {
            a = 1;
            continue;
        }
        //if user types "List" 
        else if (command == 1) {
            printf("List\n");
	    continue;
        }
        //if user types "Suspend"
        else if (command == 2) {
            printf("Please enter the input pid you want suspend\n");
	    scanf("%d" , &suspid);
	    Suspend(suspid);
            continue;
        }
        //if user types "Resume"
        else if (command == 3) {
            printf("PLease enter the input pid you want resume\n");
	    scanf("%d", &respid);
	    Resume(respid);
            continue;
        }
        // if user types "Terminate"
        else if (command == 4) {
            printf("PLease enter the input pid you want terminate\n");
	    scanf("%d", &terpid);
	    Terminate(terpid);
            continue;
        }
    }
    printf("Exited.\n");
    return 0;
 
}
