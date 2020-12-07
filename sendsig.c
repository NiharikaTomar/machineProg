////////////////////////////////////////////////////////////////////////////////
// Main File:        p6
// This File:        sendsig.c
// Other Files:      intdate.c, division.c
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
#include <signal.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    //handles error if more than 3 inputs from command line
    if(argc != 3 )
    {
        printf("Usage: <signal type> <pid>\n");
        exit(1);
        
    }
    int sigI = strcmp(argv[1], "-i" ); //compares to string "-i"
    int sigU = strcmp(argv[1], "-u" ); //compares to string "-u"
    pid_t pid = atoi(argv[2]); //stores pid number
    
    //handles if user input is -u
    if (sigU == 0)
    {
        //send signals SIGUSR1
        kill(pid, SIGUSR1);
    }
    //handles if user input is -i
    if(sigI == 0)
    {
        //send signals SIGINT
        kill(pid, SIGINT);
    }
}

