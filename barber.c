#include "waitingRoom/waitingRoom.h"
#include "moneyTransfer/moneyTransfer.h"
#include "chairs/chairs.h"
#include "cashRegister/cashRegister.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define BARBER_SLEEP 1


void cutHair();


int main(int argc, char **argv)
{
    size_t id, clientID;
    long chairNumber;
    Money money = {0, 0, 0};
	
	if (argc < 2) {
		perror("Za malo argumentow (fryzjer)");
		exit(1);
	}

	id = atol(argv[1]);
	
	printf("Fryzjer %zu rozpoczyna dzialanie\n", id);
	
    getAccessToWaitingRoom();
    getAccessToMoneyTransfer();
    getAccessToChairs();
    getAccessToCashRegister();

    while (1) {
        clientID = getNextClient();
		printf("Nastepny klient fryzjera %zu: %zu\n", id, clientID);
        chairNumber = takeChair();
		printf("Fryzjer %zu zajmuje fotel %zu\n", id, chairNumber);
		letClientPay(clientID);
		printf("Fryzjer %zu czeka na platnosc od klienta %zu\n", id, clientID);
        getMoneyForService(clientID, &money);
		printf("Fryzjer %zu przyjmuje platnosc\n", id);
        putMoneyToCashRegister(&money);
		printf("Fryzjer %zu wplaca do kasy\n", id);
		printCashRegister();
        cutHair();
		printf("Fryzjer %zu obcina klienta %zu\n", id, clientID);
        returnChair(chairNumber);
		printf("Fryzjer %zu zwalnia fotel %zu\n", id, chairNumber);
		getChangeFromCashRegister(id, &money);
		printf("Fryzjer %zu pobiera reszte\n", id);
		printCashRegister();
        giveChange(clientID, &money);
		printf("Fryzjer %zu wydaje reszte\n", id);
    }
}


void cutHair()
{
    sleep(BARBER_SLEEP);
}