#ifndef ENERGYFILTER_HH_
#define ENERGYFILTER_HH_

#include "Table.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EnergyFilter : public Table
{
public:
  EnergyFilter(const TGWindow * p, const TGWindow * main, char *name, 
	       int columns, int rows, int NumModules);
  virtual ~EnergyFilter();
  
  Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
	
  int load_info(Long_t mod);
  int change_values(Long_t mod);

protected:
  TGNumberEntry* filterRange ;

  unsigned short modNumber;
  unsigned int  fRange;
  bool Load_Once;
  float risetime, flattop;
  unsigned short chanNumber;
  TGNumberEntry* chanCopy; 
};

#endif /*ENERGYFILTER_HH_*/