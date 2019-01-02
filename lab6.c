#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include "ezipc.h"


/*Benjamin Mouer
 * CSC 241
 * Lab 6
 * 11/28/18
 * Dr.Cañas
 * */

/*This is to read the input file*/
const char * file = "unisex.txt";


int main() {
/*All of this is just creating semaphores and SM and making sure I initialized everything for correct use*/
    FILE *ifp;
    SETUP();

    int *stall_avaliable = SHARED_MEMORY(1);
    int *fem_waiting = SHARED_MEMORY(1);
    int *male_waiting = SHARED_MEMORY(1);
    int *male_inside = SHARED_MEMORY(1);
    int *fem_inside = SHARED_MEMORY(1);
    int *male_total = SHARED_MEMORY(1);
    int *fem_total = SHARED_MEMORY(1);

    *fem_waiting = 0;
    *male_waiting = 0;
    *male_inside = 0;
    *fem_inside = 0;
    *male_total = 0;
    *fem_total = 0;

    /*These varables will represent instances of the individual processes since they are not SM or Sems*/
    int eof = 1;
    int time;
    int male_id = 0;
    int fem_id = 0;
    int procid;
    int stalls;
    int waitTime;
    char gender;
    int save_status;

/*This is a simple check for my input file*/
    ifp = fopen(file, "r");
    if (ifp == NULL) {
        printf("File is not found. \n", file);
        exit(1);
    }

/*On the lab it says two different things, read in the num of stalls from file or ask the user to input the number
 * I decided to let the user input the number*/

    printf("Enter the number of stalls in the bathroom: \n");
    scanf("%d", &stalls);
    int stall = SEMAPHORE(SEM_COUNTING, stalls);
    *stall_avaliable = stalls;

/*This while, scan, and if are to make sure that as long as the EOF has not been reached, keep grabbing the gender and time or arrival for the customers.*/
    while (eof != EOF) {
        eof = fscanf(ifp, "%d %c", &time, &gender);
        if (eof != EOF) {





/*So I read the gender first and increment the male_id since it only matters to the specific process
 * Then I fork to create that process*/
            if (gender == 'M') {
                male_id += 1;
                procid = fork();

/*After I fork, I let the process sleep until the time that the customer would arrive.
 * This ensures that the customers arrive in order.*/
                if (procid == 0) {
                    sleep(time);
                    printf("Male %d arrives at time %d \n", male_id, time);
                    *male_total += 1;
                    /*If females are in the bathroom at all (whether in a stall, waiting for a stall, or washing hands) then the me will wait till there are no women in the bathroom.*/
                    if (*fem_inside > 0) {

                        printf("Male %d waits outside bathroom, women are using it \n", male_id);
                    }
                    while (*fem_inside > 0);
                    printf("Male %d enters bathroom, there were %d men inside the bathroom as he entered \n", male_id, *male_inside);
                    *male_inside += 1;
                    /*There are either stalls avaliable or not, if there are, then the customer will enter the stall immediately
                     * if not, then the customer will wait and output the number of people waiting before him. Then he will enter when one stall is free.*/
                    if (*stall_avaliable > 0) {
                        printf("Male %d uses a stall \n", male_id);
                        P(stall);
                        *stall_avaliable -= 1;
                        waitTime = time * 3 % 4 + 1;
                        sleep(waitTime);
                        V(stall);
                        printf("Male %d leaves a stall \n", male_id);
                        *stall_avaliable += 1;
                        /*The lab said that a person may spend extra time in the bathroom after leaving the stall, probably to wash hands.*/
                        sleep(waitTime);
                        *male_inside -= 1;
                        printf("Male %d leaves the bathroom, %d men are still inside\n", male_id, *male_inside);
                    } else {
                        /*this else was already explained: if a person has to wait for a stall, then output the line and enter when one is free. */
                        printf("Male %d waits in line for a stall, there are %d in line before him \n",
                               male_id,
                               *male_waiting);
                        *male_waiting += 1;
                        P(stall);
                        *male_waiting -= 1;
                        printf("Male %d uses a stall \n", male_id);
                        *stall_avaliable -= 1;
                        waitTime = ((((time) * 3) % 4) + 1);
                        sleep(waitTime);
                        V(stall);
                        printf("Male %d leaves a stall \n", male_id);
                        *stall_avaliable += 1;
                        sleep(waitTime);
                        *male_inside -= 1;
                        printf("Male %d leaves the bathroom, %d men are still inside\n", male_id, *male_inside);
                    }
                    /*We make sure to exit after a customer leaves as to not leave any useless processes running.*/
                    exit(0);
                }






/*The entire block for females is exactly the same as males functionality-wise, we just make sure its labeled females.*/
            } else if (gender == 'F') {
                fem_id += 1;
                procid = fork();
                if (procid == 0) {
                    sleep(time);
                    printf("Female %d arrives at time %d \n", fem_id, time);
                    *fem_total += 1;
                    if (*male_inside > 0) {
                        printf("Female %d waits outside bathroom, men are using it \n", fem_id);
                    }
                    while (*male_inside > 0);
                    printf("Female %d enters bathroom, there were %d women inside the bathroom as she entered \n", fem_id, *fem_inside);
                    *fem_inside += 1;
                    if (*stall_avaliable > 0) {
                        printf("Female %d uses a stall \n", fem_id);
                        P(stall);
                        *stall_avaliable -= 1;
                        waitTime = ((((time) * 3) % 4) + 1);
                        sleep(waitTime);
                        V(stall);
                        printf("Female %d leaves a stall \n", fem_id);
                        *stall_avaliable += 1;
                        sleep(waitTime);
                        *fem_inside -= 1;
                        printf("Female %d leaves the bathroom, %d women are still inside \n", fem_id, *fem_inside);
                    } else {
                        printf("Female %d waits in line for a stall, there are %d in line before her \n",
                               fem_id,
                               *fem_waiting);
                        *fem_waiting += 1;
                        P(stall);
                        printf("Female %d uses a stall \n", fem_id);
                        *fem_waiting -= 1;
                        *stall_avaliable -= 1;
                        waitTime = ((((time) * 3) % 4) + 1);
                        sleep(waitTime);
                        V(stall);
                        printf("Female %d leaves a stall \n", fem_id);
                        *stall_avaliable += 1;
                        sleep(waitTime);
                        *fem_inside -= 1;
                        printf("Female %d leaves the bathroom, %d women are still inside\n", fem_id, *fem_inside);
                    }
                    exit(0);
                }
            }
        }
    }

    /*We need this while loop so that the parent does terminate before ANY of the children terminate.
     * and finally we output our results, which come from our shared memory.*/
    while ((procid = wait(&save_status)) != -1);
    printf("\n \n \n%d total females used the bathroom. \n", *fem_total);
    printf("%d total males used the bathroom. \n", *male_total);
    printf("\n \nPROGRAM TERMINATED. \n");

    exit(1);

}