#ifndef _ORDERING_TABLE_H_
#define _ORDERING_TABLE_H_

#include "Global.h"

class OrderingTable
{
  public:  

  OrderingTable(int nEntries);
  ~OrderingTable();

  void Clear();
  void IssueToGpu();
  FORCE_INLINE void AddPrim(void* pPrim, int pos) { if (pos > 0 && pos < m_nEntries) ::AddPrim(m_pEntries + pos, pPrim); }
  void AddOt(OrderingTable* pToAdd, int pos);

  u_long* m_pEntries;
  int m_nEntries;
  bool m_bStatic;
};

#endif
