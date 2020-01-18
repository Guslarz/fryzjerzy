#ifndef CASH_REGISTER_H
#define CASH_REGISTER_H

#include "../money/money.h"

#include <stdlib.h>


/*
 *  CASH REGISTER
 */
void getAccessToCashRegister();
void initCashRegister();
void clearCashRegister();
void printCashRegister();
/*
 *  FOR BARBER
 */
void putMoneyToCashRegister(const Money*);
void getChangeFromCashRegister(size_t, Money*);
 

#endif