// main.cc --- 
// 
// Description: 
// Author: Hongyi Wu(吴鸿毅)
// Email: wuhongyi@qq.com 
// Created: 一 8月 15 16:53:40 2016 (+0800)
// Last-Updated: 一 9月  5 10:54:10 2016 (+0800)
//           By: Hongyi Wu(吴鸿毅)
//     Update #: 12
// URL: http://wuhongyi.github.io 

#include "Manager.hh"

#include <iostream>
#include "pixie16app_defs.h"
#include "pixie16app_export.h"
#include "pixie16sys_export.h"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc, char *argv[])
{

  Manager *manager = new Manager();

  manager->BootSystem();
  manager->RunManager();

  delete manager;
  return 0;
}

// 
// main.cc ends here
