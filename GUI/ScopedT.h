#ifndef SCOPEDT_H_
#define SCOPEDT_H_

#include "Table.h"

class ScopedT : public Table
{
public:
  ScopedT(const TGWindow * p, const TGWindow * main, char *name, int columns, int rows, int NumModules);
  virtual ~ScopedT();

  int change_values(Long_t mod);
  int load_info(Long_t mod);
  
protected:
  Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
  
  TGNumberEntry *chanCopy;
  short int chanNumber;
  short int modNumber;
  char tmp[10];

  bool Load_Once;
  float decay;
};


#endif /*SCOPEDT_H_*/
