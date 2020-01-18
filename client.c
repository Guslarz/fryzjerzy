#include "params.h"
#include "money/money.h"
#include "waitingRoom/waitingRoom.h"
#include "moneyTransfer/moneyTransfer.h"

#include <stdio.h>
#include <unistd.h>


#define CLIENT_SLEEP 1


void earnMoney(Money*);


int main(int argc, char **argv)
{
    size_t id;
    Money money = {0, 0, 0};

    if (argc < 2) {
        perror("Za malo argumentow (klient)");
        exit(1);
    }
	
    id = atol(argv[1]);
	
	printf("Klient %zu rozpoczyna dzialanie\n", id);
	
    getAccessToWaitingRoom();
    getAccessToMoneyTransfer();

    while (1) {
        earnMoney(&money);
		printf("Klient %zu zarobil pieniadze\n", id);
        if (!tryTakeEmptySeat(id)) {
			printf("Klient %zu nie zdolal zajac miejsca\n", id);
            continue;
		}
		printf("Klient %zu zajal miejsce\n", id);
        payForService(id, &money);
		printf("Klient %zu zaplacil\n", id);
        takeChange(id, &money);
		printf("Klient %zu odebral reszte\n", id);
    }
}


void earnMoney(Money *money)
{
    do {
		addSalary(money);
        sleep(CLIENT_SLEEP);
    } while (money->sum < SERVICE_PRICE);
}