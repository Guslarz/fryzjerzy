#ifndef MONEY_TRANSFER_H
#define MONEY_TRANSFER_H

#include "../money/money.h"
#include <stdlib.h>

/*
 *  MONEY TRANSFER
 */
void getAccessToMoneyTransfer();
void initMoneyTransfer();
void clearMoneyTransfer();
/*
 *  FOR BARBER
 */
void getMoneyForService(size_t, Money*);
void giveChange(size_t, const Money*);
/*
 *  FOR CLIENT
 */
void payForService(size_t, Money*);
void takeChange(size_t, Money*);


#endif