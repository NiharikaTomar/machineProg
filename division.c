////////////////////////////////////////////////////////////////////////////////
// Main File:        p6
// This File:        division.c
// Other Files:      intdate.c, sendsig.c
// Semester:         CS 354 spring 2019
//
// Author:           Niharika Tomar
// Email:            ntomar@wisc.edu
// CS Login:         tomar
//
/////////// IF PAIR PROGRAMMING IS ALLOWED, COMPLETE THIS SECTION //////////////
//
// Pair Partner:     (name of your pair programming partner)
// Email:            (email address of your programming partner)
// CS Login:         (partner's CS login name)
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of
//                   of any information you find.
//////////////////////////// 80 columns wide ///////////////////////////////////

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

int count_of_operations = 0; //# of times the operation has been done.

/* called and handles error when division by 0 is attempted */
void handler_divison_zero()
{
    printf("Error: a division by 0 operation was attempted.\n");
    printf("Total number of operations completed successfully: %d\n",
           count_of_operations);
    printf("The program will be terminated.\n");
    exit(0);
}

/* Called when user inputs CTRL + C  and exits program with an message */
void handler_CTRLC()
{
    printf("\nTotal number of operations successfully completed: %d\n",
           count_of_operations);
    printf("The program will be terminated.\n");
    exit(0);
}

int main()
{
    int input1, input2; //stores the number (INT) inputs from user.
    char input[100]; //stores input from user.
    
    struct sigaction sigfpe_act; //Called when divison by zero.
    struct sigaction sigint_act; //Called when CRTL C is pressed.
    
    //Makes sure the initialized the sigaction structs are zeroed out
    memset(&sigfpe_act, 0, sizeof(sigfpe_act));
    memset(&sigint_act, 0, sizeof(sigint_act));
    
    //stores function to call based on handle of signal
    sigfpe_act.sa_handler = handler_divison_zero; //based on SIGFPE
    sigint_act.sa_handler = handler_CTRLC; //based on SIGINT
    
    //based on signal from user, calls appropriate handle function
    if (sigaction(SIGFPE, &sigfpe_act, NULL) != 0){
        printf("Error when setting up sigaction() for SIGFPE. \n");
    }
    
    if (sigaction(SIGINT, &sigint_act, NULL) != 0)
    {
        printf("Error when setting up sigaction() for SIGINT. \n");
    }
    
    //infinite loop
    while (1)
    {
        //gets first integer from user
        printf("Enter first integer: ");
        
        if (fgets(input, 100, stdin) != NULL)
        {
            input1 = atoi(input);
        }
        
        //gets second integer from user
        printf("Enter second integer: ");
        
        if (fgets(input, 100, stdin) != NULL)
        {
            input2 = atoi(input);
        }
        
        //prints out to screen remainder and divison of input
        printf("%d / %d is %d with a remainder of %d\n", input1, input2,
               input1/input2, input1%input2);
        count_of_operations++; //increments count while in the loop
    }
    
    return 0;
}

