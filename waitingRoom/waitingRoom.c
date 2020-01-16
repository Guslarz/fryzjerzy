#include "waitingRoom.h"
#include "../params.h"

#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdio.h>


#define EMPTY_SEAT 1
#define TAKEN_SEAT 2
#define SEAT_BUFFER_SIZE sizeof(size_t)


typedef struct {
    long type;
    size_t id;
} SeatBuffer;


int waitingRoomID;


void addToWaitingRoom(const SeatBuffer*);


void getAccessToWaitingRoom()
{
    if ((waitingRoomID = msgget(WAITING_ROOM_KEY,
    IPC_CREAT|0600)) == -1) {
        perror("Dostep do poczekalni");
        exit(1);
    }
}


void clearWaitingRoom()
{
    SeatBuffer buffer;

    while(msgrcv(waitingRoomID, &buffer,
    SEAT_BUFFER_SIZE, 0, IPC_NOWAIT) != -1);

    if (errno != ENOMSG) {
        perror("Czyszczenie poczekalni");
        exit(1);
    }
}


void addToWaitingRoom(const SeatBuffer *buf)
{
    if (msgsnd(waitingRoomID, buf, 
    SEAT_BUFFER_SIZE, 0) == -1) {
        perror("Wstawianie elementu do poczekalni");
        exit(1);
    }
}


void initWaitingRoom()
{
    SeatBuffer buffer = {EMPTY_SEAT, 0};

    if ((waitingRoomID = msgget(WAITING_ROOM_KEY,
    IPC_CREAT|IPC_EXCL|0600)) == -1) {
        getAccessToWaitingRoom();
        clearWaitingRoom();
    }

    for (size_t i = 0; i < WAITING_ROOM_SEAT_COUNT; ++i)
        addToWaitingRoom(&buffer);
}


size_t getNextClient()
{
    SeatBuffer buffer;

    if (msgrcv(waitingRoomID, &buffer,
    SEAT_BUFFER_SIZE, TAKEN_SEAT, 0) == -1) {
        perror("Odebranie klienta z poczekalni");
        exit(1);
    }

    buffer.type = EMPTY_SEAT;
    if (msgsnd(waitingRoomID, &buffer,
    SEAT_BUFFER_SIZE, 0) == -1) {
        perror("Zwolnienie miejsca w poczekalni");
        exit(1);
    }

    return buffer.id;
}


bool tryTakeEmptySeat(size_t id)
{
    SeatBuffer buffer;
    if (msgrcv(waitingRoomID, &buffer, 
    SEAT_BUFFER_SIZE, EMPTY_SEAT, IPC_NOWAIT) == -1) {
        if (errno == ENOMSG) return false;
        else {
            perror("Znalezienie wolnego miejsca");
            exit(1);
        }
    }

    buffer.type = TAKEN_SEAT;
    buffer.id = id;
    if (msgsnd(waitingRoomID, &buffer,
    SEAT_BUFFER_SIZE, 0) == -1) {
        perror("Zajecie wolnego miejsca");
        exit(1);
    }
    return true;
}