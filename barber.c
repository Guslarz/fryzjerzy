#include "waitingRoom/waitingRoom.h"
#include "moneyTransfer/moneyTransfer.h"
#include "chairs/chairs.h"
#include "cashRegister/cashRegister.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define BARBER_SLEEP 1


void cutHair(size_t);


int main(int argc, char **argv)
{
    size_t id, clientID;
    long chairNumber;
    Money money = {0, 0, 0};
	
	if (argc < 2) {
		perror("Za malo argumentow (fryzjer)");
		exit(1);
	}

	id = itoa(argv[1]);
    getAccessToWaitingRoom();
    getAccessToMoneyTransfer();
    getAccessToChairs();
    getAccessToCashRegister();

    while (1) {
        clientID = getNextClient();
        chairNumber = takeChair();
        getMoneyForService(clientID, &money);
        putMoneyToCashRegister(&money);
        cutHair(clientID);
        returnChair(chairNumber);
		getChangeFromCashRegister(&money);
        giveChange(clientID, &money);
    }
}


void cutHair(size_t clientID)
{
    sleep(BARBER_SLEEP);
}