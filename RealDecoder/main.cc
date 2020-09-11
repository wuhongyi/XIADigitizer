// main.cc --- 
// 
// Description: 
// Author: Hongyi Wu(吴鸿毅)
// Email: wuhongyi@qq.com 
// Created: 三 2月 26 20:42:35 2020 (+0800)
// Last-Updated: 五 9月 11 20:10:29 2020 (+0800)
//           By: Hongyi Wu(吴鸿毅)
//     Update #: 36
// URL: http://wuhongyi.cn 

// wugroot main.cc  -lrt  -o 123

#include "UserDefine.hh"
#include "decoding.hh"

#include "TSystem.h"
#include "THttpServer.h"
#include "TH1.h"
#include "TH2.h"
#include "TString.h"
#include "TRandom.h"

#include <fcntl.h>
#include <semaphore.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <errno.h>
#include <unistd.h>
using namespace std;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc, char *argv[])
{
  TH1I *adc[MODULE_NUM_MAX][16];
  for (int i = 0; i < MODULE_NUM_MAX; ++i)
    for (int j = 0; j < 16; ++j)
      {
	adc[i][j] = new TH1I(TString::Format("ADC_%02d_%02d",i,j).Data(),"",ADC_HIST_BIN,ADC_HIST_MIN,ADC_HIST_MAX);
	adc[i][j]->SetDirectory(0);
      }

  
  if (gSystem->AccessPathName("auth.txt") != 0)
    {
      std::cout<<"Please start macro from directory where auth.txt file is available\n"<<std::endl;
      std::cout<<"It required to supply authentication information for the http server\n"<<std::endl;
      return 0;
    }

  THttpServer* serv = new THttpServer("http:8080?top=GDDAQ Online&auth_file=auth.txt&auth_domain=root");
  // serv->SetJSROOT("https://root.cern.ch/js/latest/");

  for (int i = 0; i < MODULE_NUM_MAX; ++i)
    for (int j = 0; j < 16; ++j)
      {
	serv->Register("ADC",adc[i][j]);
      }

  
  for (int i = 0; i < MODULE_NUM_MAX; ++i)
    for (int j = 0; j < 16; ++j)
      {
	serv->RegisterCommand(TString::Format("/Control/ADC/ADC_%02d_%02d",i,j).Data(),TString::Format("/Objects/ADC/ADC_%02d_%02d/->Reset()",i,j).Data(), "rootsys/icons/ed_delete.png");
	serv->SetItemField(TString::Format("/Control/ADC/ADC_%02d_%02d",i,j).Data(),"_update_item", TString::Format("ADC_%02d_%02d",i,j).Data()); // let browser update histogram view after commands execution
      }


  serv->Restrict("/Control",  "visible=admin");
  // serv->Restrict("/Control", "hidden=guest");
  serv->RegisterCommand("/Control/Exit","gSystem->Exit()");

  serv->SetItemField("/","_monitoring","5000");
  serv->SetItemField("/ADC/","_drawopt","");
  
  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  unsigned int data;
  decoding *rawdec;
  rawdec = new decoding[MODULE_NUM_MAX];

  
  int nloop = 0;//the number of loop
  unsigned short ModNum = 0;
  
  int shm_id_modinfo;
  unsigned char *ptr_modinfo;
  shm_id_modinfo=shm_open("shmpixie16pkuxiadaq",O_RDWR,0);/*第一步：打开共享内存区*/
  if (shm_id_modinfo == -1)
    {
      printf( "open shared memory error.errno=%d,desc=%s.\n", errno, strerror(errno));
      return -1;
    }
  else
    {
      printf( "open shared memory ok.\n");
    }
  ftruncate(shm_id_modinfo,(off_t)(SHAREDMEMORYDATAOFFSET+PRESET_MAX_MODULES*2+PRESET_MAX_MODULES*4*SHAREDMEMORYDATASTATISTICS+PRESET_MAX_MODULES*4*SHAREDMEMORYDATAENERGYLENGTH*SHAREDMEMORYDATAMAXCHANNEL));
  ptr_modinfo = (unsigned char*)mmap(NULL,SHAREDMEMORYDATAOFFSET+PRESET_MAX_MODULES*2+(PRESET_MAX_MODULES*SHAREDMEMORYDATASTATISTICS*4)+PRESET_MAX_MODULES*4*SHAREDMEMORYDATAENERGYLENGTH*SHAREDMEMORYDATAMAXCHANNEL,PROT_READ|PROT_WRITE,MAP_SHARED,shm_id_modinfo,0);/*连接共享内存区*/


  int shm_id;
  unsigned char *ptr;

  shm_id=shm_open("shmpixie16pkuxiadaqdecoderonline",O_RDWR,0);/*第一步：打开共享内存区*/
  if (shm_id == -1)
    {
      printf( "open shared memory error.errno=%d,desc=%s.\n", errno, strerror(errno));
      return -1;
    }
  else
    {
      printf( "open shared memory ok.\n");
    }
  ftruncate(shm_id,(off_t)(60+BUFFLENGTH*MODULE_NUM_MAX*4));
  ptr = (unsigned char*)mmap(NULL,60+BUFFLENGTH*MODULE_NUM_MAX*4,PROT_READ|PROT_WRITE,MAP_SHARED,shm_id,0);/*连接共享内存区*/  

  
  int oldrun = -1;
  int run;
  int datalength;

  for (;;)
    {
      memcpy(&run,ptr,sizeof(int));

      if(oldrun != run)
	{
	  memcpy(&ModNum,ptr_modinfo+4,2);
	  for (int i = 0; i < ModNum; ++i)
	    {
	      short tempsampingrate;
	      memcpy(&tempsampingrate,ptr_modinfo+SHAREDMEMORYDATAOFFSET+2*i,2);
	      rawdec[i].setsamplerate(tempsampingrate);
	      rawdec[i].init();
	    }

	  oldrun = run;
	  printf("run: %d\n",run);
	  
#ifdef CLEAR_IN_NEW_RUN
	  for (int i = 0; i < MODULE_NUM_MAX; ++i)
	    for (int j = 0; j < 16; ++j)
	      {
		adc[i][j]->Reset("ICES");
	      }
#endif
	}

      for (int i = 0; i < ModNum; ++i)
	{
	  memcpy(&datalength,ptr+4*(i+1),sizeof(int));
	  if(datalength > 0)
	    {
	      // 数据解码/填充
	      for (int j = 0; j < datalength; ++j)
		{
		  // ptr+60+BUFFLENGTH*i*4+4*j
		  memcpy(&data,ptr+60+BUFFLENGTH*i*4+4*j,sizeof(unsigned int));
		  if(rawdec[i].readword(data))
		    {
		      adc[i][rawdec[i].getch()]->Fill(rawdec[i].getevte());
		    }
		}
	      
	      datalength = 0;
	      memcpy(ptr+4*(i+1),&datalength,sizeof(int));
	    }
	}
      
      


      // for (int i = 0; i < 1000; ++i)
      // 	{
      // 	  int tmpi = int(gRandom->Uniform(0,13));
      // 	  int tmpj = int(gRandom->Uniform(0,16));
      // 	  adc[tmpi][tmpj]->Fill(gRandom->Uniform(0,65536));
      // 	}

      
      
      nloop++;
      if(nloop%100 == 0) if(gSystem->ProcessEvents()) break;
    }

  return 0;
}

// 
// main.cc ends here
