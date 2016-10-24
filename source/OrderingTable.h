#ifndef _ORDERING_TABLE_H_
#define _ORDERING_TABLE_H_

#include "Global.h"

struct OrderingTable
{
  u_long* pEntries;
  int nEntries;
  int bStatic;
};

void OrderingTable_Construct(OrderingTable* pSelf, int nEntries);
void OrderingTable_ConstructStatic(OrderingTable* pSelf, u_long* pMem, int nEntries);
void OrderingTable_Clear(OrderingTable* pSelf);
void OrderingTable_IssueToGpu(OrderingTable* pSelf);
void OrderingTable_AddPrim(OrderingTable* pSelf, void* pPrim, int pos);
void OrderingTable_AddOt(OrderingTable* pSelf, OrderingTable* pToAdd, int pos);
void OrderingTable_Destruct(OrderingTable* pSelf);

#endif
