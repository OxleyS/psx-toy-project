#include "OrderingTable.h"

void OrderingTable_Construct(OrderingTable* pSelf, int nEntries)
{
  pSelf->pEntries = nEntries ? malloc3(sizeof(u_long) * nEntries) : 0;
  pSelf->nEntries = 0;
  pSelf->bStatic = 0;
}

void OrderingTable_ConstructStatic(OrderingTable* pSelf, u_long* pMem, int nEntries)
{
  pSelf->pEntries = pMem;
  pSelf->nEntries = nEntries;
  pSelf->bStatic = 1;
}

void OrderingTable_Clear(OrderingTable* pSelf)
{
  ClearOTagR(pSelf->pEntries, pSelf->nEntries);
}

void OrderingTable_IssueToGpu(OrderingTable* pSelf)
{
  DrawOTag(pSelf->pEntries + (pSelf->nEntries - 1));
}

void OrderingTable_AddPrim(OrderingTable* pSelf, void* pPrim, int pos)
{
  if (pos > 0 && pos < pSelf->nEntries) AddPrim(pSelf->pEntries + pos, pPrim);
}

void OrderingTable_AddOt(OrderingTable* pSelf, OrderingTable* pToAdd, int pos)
{
  if (pos > 0 && pos < pSelf->nEntries)
    AddPrims(pSelf->pEntries + pos, pToAdd + (pToAdd->nEntries - 1), pToAdd);
}

void OrderingTable_Destruct(OrderingTable* pSelf)
{
  if (pSelf->bStatic) free(pSelf->pEntries);
}
