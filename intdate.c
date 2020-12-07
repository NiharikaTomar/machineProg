////////////////////////////////////////////////////////////////////////////////
// Main File:        p6
// This File:        intdate.c
// Other Files:      sendsig.c, division.c
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
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <string.h>


int alarm_print = 3; //count for time print out.
int SIGUSR1_count = 0; //count for number of user signals.

/* handles SIGALRM and prints the current time and date every 3 second */
static void handler_SIGALRM()
{
    time_t currentTime; //stores the current time
    time(&currentTime); //obtains time
    printf("PID: %d | Current Time: %s",getpid(), ctime(&currentTime));
    alarm(alarm_print); //re-arms the alarm
}

/* handles SIGUSR1, updates user count and prints that signal was caught */
static void handler_SIGUSR1()
{
    SIGUSR1_count++;
    printf("SIGUSR1 caught!\n");
}

/* handles SIGINT, prints that signal was recieved and exits program
 *WHEN CTRL C is pressed.
 */
static void handler_SIGINT()
{
    printf("\nSIGINT received.");
    printf("\nSIGUSR1 was received %d times. Exiting now.\n", SIGUSR1_count);
    exit(0);
}

int main(int argc, char *argv[])
{
    struct sigaction act; //Called when ALRM is called
    struct sigaction sigint_act; //Called when CRTL C is pressed
    struct sigaction sigusr_act; //Called with user defined signal
    
    //Makes sure the initialized the sigaction structs are zeroed out
    memset(&act, 0, sizeof(act));
    memset(&sigint_act, 0, sizeof(sigint_act));
    memset(&sigusr_act, 0, sizeof(sigusr_act));
    
    //prints out message before time is printed every 3 second
    printf("Pid and time will be printed every 3 seconds.\n");
    printf("Enter ^C to end the program.\n");
    
    alarm(alarm_print); //Start 3 sec loops
    
    //stores function to call based on handle of signal
    act.sa_handler = handler_SIGALRM;  //based on SIGALRM
    sigint_act.sa_handler = handler_SIGINT; //based on SIGINT
    sigusr_act.sa_handler = handler_SIGUSR1; //based on SIGUSR1
    
    //based on signal from user, calls appropriate handle function
    if(sigaction(SIGALRM, &act, NULL) < 0)
    {
        perror("sigaction -- SIGALRM");
        exit(1);
    }
    
    if(sigaction(SIGINT, &sigint_act, NULL) < 0)
    {
        perror("sigaction -- SIGINT");
        exit(1);
    }
    
    if(sigaction(SIGUSR1, &sigusr_act, NULL) != 0)
    {
        perror("sigaction -- SIGUSR1");
        exit(1);
    }
    
    //infinite loop
    while(1) {
    }
    
    return 0;
}

