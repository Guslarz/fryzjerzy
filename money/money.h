#ifndef MONEY_H
#define MONEY_H


typedef struct {
    int count1, count2, count5, sum;
} Money;


void addMoney(Money*, const Money*);
void subtractMoney(Money*, const Money*);
void resetMoney(Money*);
void copyMoney(Money*, const Money*);
void printMoney(const Money*);


#endif