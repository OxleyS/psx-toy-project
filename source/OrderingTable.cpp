#include "OrderingTable.h"

OrderingTable::OrderingTable(int nEntries)
{
  pEntries = nEntries ? (u_long*)malloc3(sizeof(u_long) * nEntries) : NULL;
  nEntries = 0;
  bStatic = 0;
}

OrderingTable::~OrderingTable()
{
  if (!bStatic) free(pEntries);
}

void OrderingTable::Clear()
{
  ClearOTagR(pEntries, nEntries);
}

void OrderingTable::IssueToGpu()
{
  DrawOTag(pEntries + (nEntries - 1));
}

void OrderingTable::AddOt(OrderingTable* pToAdd, int pos)
{
  if (pos > 0 && pos < nEntries)
    AddPrims(pEntries + pos, pToAdd + (pToAdd->nEntries - 1), pToAdd);
}
