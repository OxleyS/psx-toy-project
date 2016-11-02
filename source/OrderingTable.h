#ifndef _ORDERING_TABLE_H_
#define _ORDERING_TABLE_H_

#include "Global.h"

struct OrderingTable
{
  u_long* pEntries;
  int nEntries;
  int bStatic;
};

void OT_Construct(OrderingTable* pSelf, int nEntries);
void OT_ConstructStatic(OrderingTable* pSelf, u_long* pMem, int nEntries);
void OT_Clear(OrderingTable* pSelf);
void OT_IssueToGpu(OrderingTable* pSelf);
void OT_AddPrim(OrderingTable* pSelf, void* pPrim, int pos);
void OT_AddOt(OrderingTable* pSelf, OrderingTable* pToAdd, int pos);
void OT_Destruct(OrderingTable* pSelf);

#endif
