#include "money.h"
#include "../params.h"

#include <stdio.h>


void addMoney(Money *target, const Money *amount)
{
    target->count1 += amount->count1;
    target->count2 += amount->count2;
    target->count5 += amount->count5;
    target->sum += amount->sum;
}


void addSalary(Money *target)
{
	target->count1 += SALARY_1;
	target->count2 += SALARY_2;
	target->count5 += SALARY_5;
	target->sum += SALARY_1 + SALARY_2 + SALARY_5;
}


void subtractMoney(Money *target, const Money *amount)
{
    target->count1 -= amount->count1;
    target->count2 -= amount->count2;
    target->count5 -= amount->count5;
    target->sum -= amount->sum;
}


void resetMoney(Money *target)
{
    target->count1 = 0;
    target->count2 = 0;
    target->count5 = 0;
    target->sum = 0;
}


void copyMoney(Money *target, const Money *amount)
{
    target->count1 = amount->count1;
    target->count2 = amount->count2;
    target->count5 = amount->count5;
    target->sum = amount->sum;
}


void initMoney(Money *target, int count1, int count2, int count5)
{
	target->count1 = count1;
	target->count2 = count2;
	target->count5 = count5;
	target->sum = count1 + count2 + count5;
}


void printMoney(const Money *money)
{
    printf("Suma: %d\t1: %d\t2: %d\t5: %d\n",
        money->count1, money->count2, money->count5,
        money->sum);
}