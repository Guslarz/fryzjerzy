#include "chairs.h"
#include "../params.h"
#include "../keys.h"

#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


#define EMPTY_CHAIR 1
#define CHAIR_BUFFER_SIZE 0


typedef struct {
    long number;
} ChairBuffer;


int chairsID;


void addChair(const ChairBuffer*);


void getAccessToChairs()
{
    if ((chairsID = msgget(CHAIRS_KEY, 
    IPC_CREAT|0600)) == -1) {
        perror("Dostep do foteli");
        exit(1);
    }
}


void clearChairs()
{
    ChairBuffer buffer;

    while (msgrcv(chairsID, &buffer, CHAIR_BUFFER_SIZE,
    0, IPC_NOWAIT) != -1);

    if (errno != ENOMSG) {
        perror("Czyszczenie foteli");
        exit(1);
    }
}


void addChair(const ChairBuffer *buffer)
{
    if (msgsnd(chairsID, buffer, 
    CHAIR_BUFFER_SIZE, 0) == -1) {
        perror("Wstawianie krzesla");
        exit(1);
    }
}


void initChairs()
{
    ChairBuffer buffer;

    if ((chairsID = msgget(CHAIRS_KEY, 
    IPC_CREAT|IPC_EXCL|0600)) == -1) {
        getAccessToChairs();
        clearChairs();
    }

    for (long i = 0; i < CHAIR_COUNT; ++i) {
        buffer.number = i + 1;
        addChair(&buffer);
    }
}


long takeChair()
{
    ChairBuffer buffer;

    if (msgrcv(chairsID, &buffer, 
    CHAIR_BUFFER_SIZE, 0, 0) == -1) {
        perror("Pobranie wolnego fotela");
        exit(1);
    }
    return buffer.number;
}


void returnChair(long number)
{
    ChairBuffer buffer = {number};

    if (msgsnd(chairsID, &buffer, 
    CHAIR_BUFFER_SIZE, 0) == -1) {
        perror("Zwrot fotela");
        exit(1);
    }
}