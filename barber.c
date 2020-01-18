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
	
	printf("Fryzjer %zu\n", id);
	
    getAccessToWaitingRoom();
    getAccessToMoneyTransfer();
    getAccessToChairs();
    getAccessToCashRegister();

    while (1) {
        clientID = getNextClient();
		printf("Nastepny klient fryzjera %zu: %zu\n", id, clientID);
		fflush(stdout);
        chairNumber = takeChair();
		printf("Fryzjer %zu zajmuje fotel %zu\n", id, chairNumber);
		fflush(stdout);
		letClientPay(clientID);
		printf("Fryzjer %zu czeka na platnosc od klienta %zu\n", id, clientID);
        getMoneyForService(clientID, &money);
		printf("Fryzjer %zu przyjmuje platnosc\n", id);
		fflush(stdout);
        putMoneyToCashRegister(&money);
		printf("Fryzjer %zu wplaca do kasy\n", id);
		fflush(stdout);
        cutHair();
		printf("Fryzjer %zu obcina klienta %zu\n", id, clientID);
		fflush(stdout);
        returnChair(chairNumber);
		printf("Fryzjer %zu zwalnia fotel %zu\n", id, chairNumber);
		fflush(stdout);
		getChangeFromCashRegister(&money);
		printf("Fryzjer %zu pobiera reszte\n", id);
		fflush(stdout);
        giveChange(clientID, &money);
		printf("Fryzjer %zu wydaje reszte\n", id);
		fflush(stdout);
    }
}


void cutHair()
{
    sleep(BARBER_SLEEP);
}