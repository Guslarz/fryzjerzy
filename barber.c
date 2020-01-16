#include "waitingRoom/waitingRoom.h"
#include "moneyTransfer/moneyTransfer.h"
#include "chairs/chairs.h"
#include "cashRegister/cashRegister.h"

#include <stdio.h>
#include <unistd.h>


#define BARBER_SLEEP 1


void cutHair(size_t);


int main(int argc, char **argv)
{
    size_t clientID;
    long chairNumber;
    Money money = {0, 0, 0};

    getAccessToWaitingRoom();
    getAccessToMoneyTransfer();
    getAccessToChairs();
    getAccessToCashRegister();

    while (1) {
        clientID = getNextClient();
        chairNumber = takeChair();
        getMoneyForService(clientID, &money);
        //do kasy
        cutHair(clientID);
        returnChair(chairNumber);
        giveChange(clientID, &money);
    }
}


void cutHair(size_t clientID)
{
    sleep(BARBER_SLEEP);
}