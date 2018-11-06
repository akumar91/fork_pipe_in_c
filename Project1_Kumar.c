/* 
  
   Author 		: Ankit Kumar
   ID     		: 009072
   Class  		: CSCI 474
   Instructor	: Dr. Jun Kong
   Project		: Project 1 ( Fork  and Pipe)
  
   
*/

#include<stdio.h>
#include<time.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/time.h>
#include<stdlib.h>
main()
{
	
        int totalChildren=4;					// Set total children default to max number of children possible
        int fileNumber=1;						// Set default file to file1
        char *inputFile;						// char pointer to the file name
        int totalLines=0;						// Holds the total number of lines in the file
        int segmentLength=0;					// The length on which each child will operate
        int segmentBound[2];					// An Array that keeps track of position where each child will operate
        
		int i=0;								// For loop iteration variables
		int j=0;
		int k=0;
        
		int fdChild2Parent[totalChildren][2];	// file descriptor to pass information from Child to Parent
        int fdParent2Child[totalChildren][2];	// file descriptor to pass information from Parent to Child
        
        int totalSum=0;							// Holds the total sum of all numbers from the file
        int childSum=0;							// Holds the total sum from each individual child
        			
        
	    int child;								// Holds the number of chidren  user wants
        char fileN[256];						// Holds the file name user wants to opearte on
												// We are using char here as we will be using it in fgets later on
	

        pid_t childID;							// Holds the ID of the new child created
		
		// variables for tracking time
		struct timeval timeTracker;
		int startTime;
		int finishTime;
		int totalRunningTime;
		
		
		printf("How many child processes do you want to use? (1,2,4)\n");	// Asking the number of child process
		scanf("%d",&child);
		switch(child)
		{
			case 1:
			totalChildren=1;
			break;
			case 2:
			totalChildren=2;
			break;
			case 4: 
			totalChildren=4;
			break;
			default:
			printf("Invalid Choice, No of children is set to 1\n");			// Set defualt to one in case of unexpected input
			totalChildren=1;
		}
													
		printf("You have chosen to use %d children\n",totalChildren);
		
        
        printf("Which file do you want to use? (1,2,3,4)\n");			   // Ask user to input which file they want to use
        scanf("%s",&fileN);
        fileNumber=atoi(fileN);											   // Converting char to int

        switch(fileNumber)												   // Assign file details based on user input
        {
           case 1:
                totalLines=1000;
            	inputFile="file1.dat";
				break;
		
		case 2:
				totalLines=10000;
				inputFile="file2.dat";
				break;
		
		case 3:
				totalLines=100000;
				inputFile="file3.dat";
				break;

		case 4:
				totalLines=1000000;
				inputFile="file4.dat";
				break;
		
		default:															// Assign file 1 in case of unexpected input
				totalLines=1000;
                inputFile="file1.dat";
              	break;
	}
 	printf("You have chosen %s that has %d records\n",inputFile,totalLines);
	 	
    	 
	segmentLength=totalLines/totalChildren;								  	// calculating the length of segment each child will operate on
	printf("Each child will operate on %d lines\n",segmentLength);
	
 	
 	
	for(i=0;i<totalChildren;i++)
	{	
		// we are assigning segment bounds for segments on which children 
		// will operate.
		// lower segment bound for a child
		segmentBound[0] = i*segmentLength;
		
		// upper segment bound for a child
		segmentBound[1]=(i+1)*segmentLength;
			
		// initializing the pipes		
		pipe(fdChild2Parent[i]);
		pipe(fdParent2Child[i]);
		
		


		// Writing the segment bound information to the pipe which will be passed to the children
		write(fdParent2Child[i][1],segmentBound,2*sizeof(totalChildren));
		
	}
		// Calculating the time
	 	// function that gets the time of the day
		gettimeofday(&timeTracker,NULL);
		// assigns startTime the time of the day converted in microseconds
		startTime=timeTracker.tv_usec + (1000000 * timeTracker.tv_sec);
		// creating children and operating on the file
	for(j=0;j<totalChildren;j++)
	{
		
		// current bounds will hold the information on which segments to 
		// operate on information given by fdParent2Child
		int currentBounds[2];
		
		// forking
		if((childID=fork())==-1)				// if error while creating the child	
		{
			perror("parent");
			exit(-1);
		}
		
		if(childID==0)							// if child was succesfully created
		{	
			read(fdParent2Child[j][0],currentBounds,2*sizeof(totalChildren));
			
			
			
		
			
			
			FILE * fileUsed;
			// we are opening the file to fileUsed for read only operation
			fileUsed=fopen(inputFile,"r");
			// seek characters to start reading at a specific location, We use 5* currentBounds, as each line in our file has 5 characters
			fseek(fileUsed,currentBounds[0]*5,SEEK_SET);

			for(k=currentBounds[0];k<currentBounds[1];k++)
			{
				

				

				
				// stores the value from the file
				fgets(fileN,4,fileUsed);
				// seeks the next two characters as they are newline and return characters
				fseek(fileUsed,2,SEEK_CUR);
				// converts char to int and adds to childSum
				childSum+=atoi(fileN);
							
			}
				// writes to fdChild2Parent to pass child sums to the parent
				write(fdChild2Parent[j][1],&childSum,sizeof(totalChildren));
				// closes the file
				fclose(fileUsed);
				exit(0);			
		}
		
	}
	// parent getting information from the children
		for(k=0;k<totalChildren;k++)
		{
			int temp;
			// reads information from fdChild2Parent into temp
			read(fdChild2Parent[k][0],&temp,sizeof(int));
			// adds all the temp values to totalSum
			totalSum+=temp;
			
			
		}
		// again gets the time of the day
		gettimeofday(&timeTracker,NULL);
		// assigns finish time the time of the day converted in microseconds
		finishTime= timeTracker.tv_usec +( 1000000*timeTracker.tv_sec);
		
		// total running time is finish time - start time
		totalRunningTime = finishTime- startTime;
	
	
	// Printing the total sum and total running time
	printf("\nTotal is %d\n", totalSum);
	printf("\nTotal Running time is: %d\n", totalRunningTime);
	
	
}	

	
