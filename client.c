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
    id = atoi(argv[1]);
    getAccessToWaitingRoom();
    getAccessToMoneyTransfer();

    while (1) {
        earnMoney(&money);
        if (!tryTakeEmptySeat(id))
            continue;
        payForService(id, &money);
        takeChange(id, &money);
    }
}


void earnMoney(Money *money)
{
    do {
        money->count1 += SALARY_1;
        money->count2 += SALARY_2;
        money->count5 += SALARY_5;
        sleep(CLIENT_SLEEP);
    } while (money->sum < SERVICE_PRICE);
}