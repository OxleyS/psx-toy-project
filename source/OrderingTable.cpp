#include "OrderingTable.h"

OrderingTable::OrderingTable(int nEntries)
{
  m_pEntries = nEntries ? (u_long*)malloc3(sizeof(u_long) * nEntries) : NULL;
  m_nEntries = nEntries;
  m_bStatic = 0;
}

OrderingTable::~OrderingTable()
{
  if (!m_bStatic) free(m_pEntries);
}

void OrderingTable::Clear()
{
  ClearOTagR(m_pEntries, m_nEntries);
}

void OrderingTable::IssueToGpu()
{
  DrawOTag(m_pEntries + (m_nEntries - 1));
}

void OrderingTable::AddOt(OrderingTable* pToAdd, int pos)
{
  if (pos > 0 && pos < m_nEntries)
    AddPrims(m_pEntries + pos, pToAdd + (pToAdd->m_nEntries - 1), pToAdd);
}
