#include "MainFrame.hh"
#include "Csra.hh"
#include "Detector.hh"
#include "Global.hh"

#include "pixie16app_export.h"
#include "pixie16sys_export.h"

#include <unistd.h>
#include <sys/stat.h>//stat(const char *file_name,struct stat *buf)
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ClassImp(MainFrame)

const char *spec_types[] =
{ "Radware spectrum", "*.spe", "all files", "*.*", 0, 0 };

const char *filetypes[] =
  { "Set Files", "*.set", "all files", "*.*", 0, 0 };

MainFrame::MainFrame(const TGWindow * p)
{
  ///initialise variables///////////////
  moduleNr = 0;
  channelNr = 0;
  size = 8192; //scope trace size
  detector = NULL; 
  runnum = 0;
  ///////////////////////////////////////
  CreateMenuBar();

  SetWindowName("PKU XIA Pixie-16 DAQ");
  MapSubwindows();
  MapWindow();
  Resize(INITIAL_WIDTH, INITIAL_HIGHT);
  //hpx_once_wave=false;
  fHpx_wave = NULL;
  trace = NULL;
  trace_float = NULL;
  //mca_float = NULL;
  //MCA_RUN = false;

  wave_once = false;
  //MCAALL = true;
  
  AppendPad(); //foarte important
  //fMca = NULL;
  xmin = 0;
  xmax = 8192;
  ymin = 0;
  ymax = 4096;
  NUMBERofTRACES = 500; //number of traces to search for waveforms
  range = 480;
  separation = 480;
  fraction = 0.0002;

  flagonlinemode = 0;
}

MainFrame::~MainFrame()
{
  cout<<"destructor of MainFrame is called!"<<endl;
}

void MainFrame::CreateMenuBar()
{
  TGMenuBar *MenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);

  MenuFile = new TGPopupMenu(fClient->GetRoot());
MenuFile->AddEntry("E&xit", FILE_EXIT,0,gClient->GetPicture("bld_exit.png"));
  MenuFile->AddSeparator();
MenuFile->AddEntry("&About", ABOUT,0,gClient->GetPicture("ed_help.png"));
  MenuFile->Associate(this);
  MenuBar->AddPopup("&File", MenuFile, new TGLayoutHints (kLHintsTop | kLHintsLeft, 0, 0, 0, 0));
  AddFrame(MenuBar, new TGLayoutHints (kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 0, 0));

  MenuSetup = new TGPopupMenu(fClient->GetRoot ());
  MenuSetup->AddEntry("&Base Setup", BASE);
  MenuSetup->AddEntry("&Trigger Filter", TFILTER);
  MenuSetup->AddEntry("&Energy", ENERGY);
  MenuSetup->AddEntry("&CFD", CFDP);
  MenuSetup->AddEntry("&QDC", QDCP);
  MenuSetup->AddEntry("&Histogramming", HISTOGRAM);
  MenuSetup->AddEntry("dT", SCOPEDT);
  MenuSetup->AddSeparator();
MenuSetup->AddEntry("Save2File", FILE_SAVE,0,gClient->GetPicture("save.xpm"));
  MenuSetup->Associate(this);
  MenuBar->AddPopup("&UV_Setup", MenuSetup, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 0, 0));
  MenuSetup->DisableEntry(BASE);
  MenuSetup->DisableEntry(ENERGY);
  MenuSetup->DisableEntry(TFILTER);
  MenuSetup->DisableEntry(CFDP);
  MenuSetup->DisableEntry(QDCP);
  MenuSetup->DisableEntry(HISTOGRAM);
  MenuSetup->DisableEntry(SCOPEDT);
  MenuSetup->DisableEntry(FILE_SAVE);

  MenuExpert = new TGPopupMenu(fClient->GetRoot());
  MenuExpert->AddEntry("Module Variables", MODVAR);
  MenuExpert->AddEntry("&CSRA", CSRA);
  MenuExpert->AddEntry("Logic Set", LOGIC);
  MenuExpert->Associate(this);
  MenuBar->AddPopup("&Expert", MenuExpert, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 0, 0));
  MenuExpert->DisableEntry(MODVAR);
  MenuExpert->DisableEntry(CSRA);
  MenuExpert->DisableEntry(LOGIC);

  // TGPopupMenu *MenuScope = new TGPopupMenu(fClient->GetRoot());
  // // MenuScope->AddEntry("xy maxmin", MAXMIN);
  // MenuScope->AddEntry("dT", SCOPEDT);
  // MenuScope->Associate(this);
  // MenuBar->AddPopup("&Scope", MenuScope, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 0, 0));

  MenuOffline = new TGPopupMenu(fClient->GetRoot());
  MenuOffline->AddEntry("Adjust Par", OFFLINEADJUSTPAR);
  MenuOffline->AddEntry("Simulation", SIMULATION);
  MenuOffline->Associate(this);
  MenuBar->AddPopup("&Offline", MenuOffline, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 0, 0));
  MenuOffline->DisableEntry(OFFLINEADJUSTPAR);
  MenuOffline->DisableEntry(SIMULATION);



  TGTab *TabPanel = new TGTab(this);
  this->AddFrame(TabPanel, new TGLayoutHints(kLHintsBottom | kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));
  TGCompositeFrame *Tab1 = TabPanel->AddTab("Oscilloscope");
  MakeFold1Panel(Tab1);

  TGCompositeFrame *Tab2 = TabPanel->AddTab("List Mode Run");
  MakeFold2Panel(Tab2);

}

void MainFrame::CloseWindow()
{
  if(detector != NULL) delete detector;
  gApplication->Terminate(0);
}

Bool_t MainFrame::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  int test1 = 0, test2 = 0;
  switch (test1 = GET_MSG(msg))
    {
    case  kC_HSLIDER:
      switch (test2 = GET_SUBMSG(msg))
	{
	case  kSL_POS:
	  switch(parm1)
	    {

	    }
	  break;

	default:
	  break;
	}
      break;
    case kC_COMMAND:
      switch (test2 = GET_SUBMSG(msg))
	{

	  ////////////////////menu popups/////////////////////////////            
	case kCM_MENU:
	  switch (parm1)
	    {
	    case FILE_EXIT:
	      CloseWindow();
	      break;

	    case ABOUT:
	     about =  new TRootHelpDialog(this, "About PKU XIA Pixie-16 DAQ", 550, 250);
	     about->SetText(gAbout);
	     about->Popup();
	      break;
	    case BASE:
	      base = new Base(fClient->GetRoot(), this, (char*)"Base Setup", 6, 16, detector->NumModules);
	      base->load_info(0);
	      break;
	    case CFDP:
	      cfd=new Cfd(fClient->GetRoot(),this,(char*)"Cfd Par.", 4, 16, detector->NumModules);
	      cfd->load_info(0);
	      break;
	    case QDCP:
	      qdc=new Qdc(fClient->GetRoot(),this,(char*)"Qdc Par.", 9, 16, detector->NumModules);
	      qdc->load_info(0);
	      break;
	    case ENERGY:
	      energy = new Energy(fClient->GetRoot(), this, (char*)"Energy", 5, 16, detector->NumModules);
	      energy->load_info(0);
	      break;
	    case CSRA:
	      csra = new Csra(fClient->GetRoot(), this,detector->NumModules);
	      csra->load_info(0);
	      break;
	    case TFILTER:
	      triggerfilter = new TriggerFilter(fClient->GetRoot (), this, (char*)"Trigger Filter", 4, 16, detector->NumModules);
	      triggerfilter->load_info(0);
	      break;
	    case MODVAR:
	      expertmod = new ExpertMod(fClient->GetRoot(), this, (char*)"Expert MOD", detector->NumModules);
	      expertmod->load_info(0);
	      break;
	    case LOGIC:
	      logictrigger = new LogicTrigger(fClient->GetRoot(),this,(char*)"Logic Trigger", 16/*7*/, 16, detector->NumModules);
	      logictrigger->load_info(0);
	      break;
	    case HISTOGRAM:
	      histogram = new Histogram(fClient->GetRoot(), this, (char*)"Histogramming", 3, 16, detector->NumModules);
	      histogram->load_info(0);
	      break;
	    case SCOPEDT:
	      scopedt = new ScopedT(fClient->GetRoot(), this, (char*)"dT", 2, 16, detector->NumModules);
	      scopedt->load_info(0);
	      break;
	    case OFFLINEADJUSTPAR:
	      popupoffline = new Offline(fClient->GetRoot(), this, detector,filepathtext,filenametext);
	      break;
	    case SIMULATION:
	      simulation = new Simulation(fClient->GetRoot(), this/*, detector,filepathtext,filenametext*/); //TODO
	      break;
	      
	    case FILE_SAVE:
	      {
	    	static TString dir2(".");
	    	TGFileInfo fInput2;
	    	fInput2.fFileTypes = filetypes;
	    	fInput2.fIniDir = StrDup(dir2);
	    	new TGFileDialog(fClient->GetRoot(), this, kFDSave, &fInput2);
	    	save_setup(fInput2.fFilename);
	    	//    cout<<"bingo\n\n\n";
	      }
	      break;

	    default:
	      break;
	    }
	case kCM_BUTTON:
	  switch (parm1)
	    {	      
	    case BOOT_BUTTON:
	      bootB->SetEnabled(0);
	      fClient->GetColorByName("red", color);
	      StateMsgFold1->SetTextColor(color, false);
	      StateMsgFold1->SetText("booting ...");
	      gSystem->ProcessEvents();
	      gPad->SetCursor(kWatch);
	      if(detector != 0) delete detector;
	      detector = new Detector(flagonlinemode);
	      if(detector->BootSystem())
		{
		  fClient->GetColorByName("green", color);
		  StateMsgFold1->SetTextColor(color, false);
		  StateMsgFold1->SetText("Booted system");
		  gPad->SetCursor(kPointer);

		  if(flagonlinemode == 0)
		    {
		      acquireB->SetEnabled(1);
		      saveB->SetEnabled(1);
		    }

		  MenuSetup->EnableEntry(BASE);
		  MenuSetup->EnableEntry(ENERGY);
		  MenuSetup->EnableEntry(TFILTER);
		  MenuSetup->EnableEntry(CFDP);
		  MenuSetup->EnableEntry(QDCP);
		  MenuSetup->EnableEntry(HISTOGRAM);
		  MenuSetup->EnableEntry(SCOPEDT);
		  MenuSetup->EnableEntry(FILE_SAVE);
		  MenuExpert->EnableEntry(MODVAR);
		  MenuExpert->EnableEntry(CSRA);
		  MenuExpert->EnableEntry(LOGIC);
		  MenuOffline->EnableEntry(OFFLINEADJUSTPAR);
		  MenuOffline->EnableEntry(SIMULATION);
		}
	      else
		{
		  fClient->GetColorByName("blue", color);
		  StateMsgFold1->SetTextColor(color, false);
		  StateMsgFold1->SetText("boot Failed...");
		  bootB->SetEnabled(1);
		}
	      break;
	    case READ_WF:
	      {
		NewTrace(size, moduleNr, channelNr);
	      }
	      break;
	    case SAVE_SEC:
	      {
		cout << "Saving to file\n" << flush;
		static TString dir1(".");
		TGFileInfo fInput1;
		fInput1.fFilename = NULL;
		fInput1.fFileTypes = spec_types;
		fInput1.fIniDir = StrDup(dir1);
		new
		  TGFileDialog(fClient->GetRoot (), this, kFDSave, &fInput1);
		if (fInput1.fFilename != NULL)
		  {
		    if (trace_float != NULL)
		      delete trace_float;
		    trace_float = new float[size];
		    for (int i = 0; i < size; i++)
		      trace_float[i] = (float) trace[i];
		    writeSpe(fInput1.fFilename, trace_float, size);
		  }
		else
		  {
		    cout << "no file name entered !\n" << flush;
		  }
	      }
	      break;
	    case MODULE_NUMBER:
	      if (parm2 == 0)
		{
		  if (moduleNr != detector->NumModules-1)
		    {
		      ++moduleNr;
		      numericMod->SetIntNumber(moduleNr);
		    }
		}
	      else
		{
		  if (moduleNr != 0)
		    {
		      if (--moduleNr == 0)
			moduleNr = 0;
		      numericMod->SetIntNumber(moduleNr);
		    }
		}
	      break;
	    case CHANNEL_NUMBER:
	      if (parm2 == 0)
		{
		  if (channelNr != 15)
		    {
		      ++channelNr;
		      numericCh->SetIntNumber(channelNr);
		    }
		}
	      else
		{
		  if (channelNr != 0)
		    {
		      if (--channelNr == 0)
			channelNr = 0;
		      numericCh->SetIntNumber(channelNr);
		    }
		}
	      break;
	      
	    default:
	      break;
	    }
	}
    case kC_TEXTENTRY:
      switch (parm1)
	{
	case MODULE_NUMBER:
	  switch (GET_SUBMSG(msg))
	    {
	    case kTE_ENTER:
	      moduleNr = numericMod->GetIntNumber();
	      numericMod->SetIntNumber(moduleNr);
	      break;
	    default:
	      break;
	    }
	case CHANNEL_NUMBER:
	  switch (GET_SUBMSG(msg))
	    {
	    case kTE_ENTER:
	      channelNr = numericCh->GetIntNumber();
	      numericCh->SetIntNumber(channelNr);
	      break;
	    default:
	      break;
	    }
	  break;

	default:
	  break;
	}

      /////////////////default for the most inclusive switch//////////          
    default:
      selected = gPad->GetSelected();
      break;
    }
  return kTRUE;
}

void MainFrame::MakeFold1Panel(TGCompositeFrame * TabPanel)
{
  TGCompositeFrame *LogoFrame = new TGCompositeFrame(TabPanel, 0, 0, kHorizontalFrame);

  TGImageMap* fImagePKU = new TGImageMap(LogoFrame, "pkulogo100.jpg");
  fImagePKU->Resize(100,100);
  fImagePKU->ChangeOptions(fImagePKU->GetOptions() | kFixedSize);
  LogoFrame->AddFrame(fImagePKU,new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, 0, 0, 0));

  TGImageMap* fImageWHY = new TGImageMap(LogoFrame, "why.jpg");
  fImageWHY->Resize(100,100);
  fImageWHY->ChangeOptions(fImageWHY->GetOptions() | kFixedSize);
  LogoFrame->AddFrame(fImageWHY,new TGLayoutHints(kLHintsTop | kLHintsRight, 100, 0, 0, 0));

  TabPanel->AddFrame(LogoFrame, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 0, 0));

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  
  //make the buttons frame        
  TGCompositeFrame *ButtonFrame = new TGCompositeFrame(TabPanel, 0, 0, kHorizontalFrame);

  // Online mode
  onlinemode = new TGCheckButton(ButtonFrame,"&Online Mode");
  fClient->GetColorByName("red", color);
  onlinemode->SetTextColor(color);
  onlinemode->SetState(kButtonDown);
  onlinemode->Connect("Clicked()","MainFrame",this,"SetOnlineMode()");
  ButtonFrame->AddFrame(onlinemode,new TGLayoutHints(kLHintsLeft|kLHintsTop,5,10,15,0));
  
  // BOOT button//////////////////////////////////////////////////////////////    
  bootB = new TGTextButton(ButtonFrame, "  Boot  ", BOOT_BUTTON);
  bootB->SetFont("-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1", false);
  fClient->GetColorByName("blue", color);
  bootB->SetTextColor(color, false);
  bootB->Associate(this);
  ButtonFrame->AddFrame(bootB, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 10,10, 0));

  TabPanel->AddFrame(ButtonFrame, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 0, 0));
  // status frame and status TGTextEntry holder//////////////////////////////////
  TGGroupFrame *StateMsgFrame = new TGGroupFrame(ButtonFrame, "Status", kVerticalFrame);

  StateMsgFold1 = new TGTextEntry(StateMsgFrame,
				  new TGTextBuffer(30), 10000,
				  StateMsgFold1->GetDefaultGC()(),
				  StateMsgFold1->GetDefaultFontStruct(),
				  kRaisedFrame | kDoubleBorder,
				  GetWhitePixel());
  StateMsgFold1->SetFont("-adobe-helvetica-bold-r-*-*-10-*-*-*-*-*-iso8859-1", false);

  fClient->GetColorByName("blue", color);
  StateMsgFold1->SetTextColor(color, false);
  StateMsgFold1->SetText("System not booted");
  StateMsgFold1->Resize(150, 12);
  StateMsgFold1->SetEnabled(kFALSE);
  StateMsgFold1->SetFrameDrawn(kFALSE);

  /////////////
  StateMsgFrame->AddFrame(StateMsgFold1, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 0, 0));
  ButtonFrame->AddFrame(StateMsgFrame, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 5, 1, 5));

  //////////////////////////////////////////////////////////////////////////
  acquireB = new TGTextButton(ButtonFrame, "Read WF", READ_WF);
  acquireB->SetEnabled(0);
  fClient->GetColorByName("purple", color);
  acquireB->SetTextColor(color, false);
  acquireB->Associate(this);
  ButtonFrame->AddFrame(acquireB, new TGLayoutHints(kLHintsLeft | kLHintsTop, 3, 10, 10, 0));
  //////////////////////////////////////////////////////////////////////////////

  saveB = new TGTextButton(ButtonFrame, "  Save  ", SAVE_SEC);
  saveB->SetEnabled(0);
  saveB->Associate(this);
  fClient->GetColorByName("purple", color);
  saveB->SetTextColor(color, false);
  saveB->SetToolTipText("Save waveform to radware .sec file", 0);
  ButtonFrame->AddFrame(saveB, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 10, 10, 0));
  ///Labels///////////////////////////////////////////////////////////////////////////
  TGVerticalFrame *fModChLabels = new TGVerticalFrame(ButtonFrame);

  TGLabel *mod = new TGLabel(fModChLabels, "Module  nr:");
  fModChLabels->AddFrame(mod, new TGLayoutHints(kLHintsLeft | kLHintsTop, 10, 3, 4, 0));
  TGLabel *ch = new TGLabel(fModChLabels, "Channel nr:");
  fModChLabels->AddFrame(ch, new TGLayoutHints(kLHintsLeft, 10, 3, 4, 0));
  ButtonFrame->AddFrame(fModChLabels, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 0, 0));
  ///Number entries/////////////////////////////////////////////////////////////////////////
  TGVerticalFrame *fModCh = new TGVerticalFrame(ButtonFrame);
  numericMod = new TGNumberEntry(fModCh, 0, 4, MODULE_NUMBER, //module number entry
				 (TGNumberFormat::EStyle) 0, (TGNumberFormat::EAttribute) 1, (TGNumberFormat::ELimit) 3/*kNELLimitMinMax*/, 0, PRESET_MAX_MODULES-1);
  numericMod->SetButtonToNum(0);
  fModCh->AddFrame(numericMod, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 3, 0, 0));
  numericMod->Associate(this);

  numericCh = new TGNumberEntry(fModCh, 0, 4, CHANNEL_NUMBER, //channel number entry
				(TGNumberFormat::EStyle) 0, (TGNumberFormat::EAttribute) 1, (TGNumberFormat::ELimit) 3/*kNELLimitMinMax*/, 0, 15);
  numericCh->SetButtonToNum(0);

  fModCh->AddFrame(numericCh, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 3, 0, 0));

  numericCh->Associate(this);
  ButtonFrame->AddFrame(fModCh, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 0, 0));

  
  ////////////////////Canvas///////////////////////////////////////////////////                                  
  TGCompositeFrame *CanvasFrame = new TGCompositeFrame(TabPanel, 60, 60, kHorizontalFrame);
  TGLayoutHints *Hint = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 1, 1, 1, 1);

  TRootEmbeddedCanvas *eCanvasF1 = new TRootEmbeddedCanvas("ec1", CanvasFrame, 100, 100);

  dCanvasF1 = eCanvasF1->GetCanvas();

  dCanvasF1->SetBorderMode(0); //no red frame
  CanvasFrame->AddFrame(eCanvasF1, Hint);
  TabPanel->AddFrame(CanvasFrame, Hint);

  test = new TGStatusBar(TabPanel, 10, 10);

  parts[0]=33;
  parts[1]=10;
  parts[2]=10;
  parts[3]=47;

  test->SetParts(parts, 4);

  TabPanel->AddFrame(test, new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 2, 2, 1, 1));

}

void MainFrame::NewTrace(unsigned long size, unsigned short module, unsigned short ChanNum)
{
  if (trace == NULL)
    trace = new unsigned short[size];

  memset(trace, 0, size * sizeof(unsigned short));

  if (fHpx_wave == NULL)
    fHpx_wave = new TH1S("hpx_wave", "Live trace", size, 0,size);
  if(fHpx_wave == NULL) cout<<"Error!"<<endl;
  detector->AcquireADCTrace(trace, size, module, ChanNum);

  gStyle->SetOptStat(0);

  fHpx_wave->Reset();
  for (int i = 0; i < (int)size; i++){
    fHpx_wave->Fill(i, trace[i]);
  }

  dCanvasF1->cd();
  fHpx_wave->DrawCopy("hist");
  dCanvasF1->Modified();
  dCanvasF1->Update();

  gSystem->ProcessEvents();

}

void MainFrame::writeSpe(const char *filename, float buf[], int dim)
{
  struct spePrefix
  {
    int reclA; /* 24 */
    unsigned titleA;
    unsigned titleB;
    int dim;
    int a1; /*  1 */
    int a2; /*  1 */
    int a3; /*  1 */
    int reclB; /* 24 */
  } x = { 24, 0, 0, 0, 1, 1, 1, 24 };
  int recl;

  //de revenit -> trebe puse sigurante !
  // char buffer[100];
  ofstream out(filename, ios::out | ios::binary);
  x.dim = dim;
  recl = sizeof(float) * dim;
  out.write(reinterpret_cast < char *>(&x), sizeof(struct spePrefix));
  out.write(reinterpret_cast < char *>(&recl), sizeof (recl));
  out.write(reinterpret_cast < char *>(buf), sizeof(float) * dim);
  out.write(reinterpret_cast < char *>(&recl), sizeof (recl));
  out.close();
}

void MainFrame::save_setup(char *name)
{
  int retval;
  retval = Pixie16SaveDSPParametersToFile(name);
  if(retval < 0)
    ErrorInfo("MainFrame.cc", "save_setup(...)", "Pixie16SaveDSPParametersToFile", retval);
  cout << "saving setup to file: " << name << endl;
}

void MainFrame::MakeFold2Panel(TGCompositeFrame *TabPanel){
 
  // Set up for file store parametrs
  TGGroupFrame *filesetgroup = new TGGroupFrame(TabPanel,"Setup");
 
  // *** FILE path frame
  TGHorizontalFrame *filepath = new TGHorizontalFrame(filesetgroup);
  TGLabel *filepathlabel = new TGLabel(filepath,"File Path: ");
  filepath->AddFrame(filepathlabel,new TGLayoutHints(kLHintsLeft | kLHintsTop, 10, 3, 4, 0));
  filepathtext = new TGTextEntry(filepath,new TGTextBuffer(120));
  filepath->AddFrame(filepathtext,new TGLayoutHints(kLHintsExpandX|kLHintsTop, 10 ,3,4,0));
  
  filesetgroup->AddFrame(filepath,new TGLayoutHints(kLHintsExpandX|kLHintsTop));

  // *** File Name frame 
  TGHorizontalFrame *filenamef = new TGHorizontalFrame(filesetgroup);

  TGLabel *filenamelabel = new TGLabel(filenamef,"File Name: ");
  filenamef->AddFrame(filenamelabel,new TGLayoutHints(kLHintsLeft | kLHintsTop, 10 ,3,4,0));
  filenametext = new TGTextEntry(filenamef, new TGTextBuffer(20));
  filenamef->AddFrame(filenametext,new TGLayoutHints(kLHintsLeft| kLHintsTop,10,3,4,0));

  TGLabel *filerunlabel = new TGLabel(filenamef,"Run Num: ");
  filenamef->AddFrame(filerunlabel,new TGLayoutHints(kLHintsLeft| kLHintsTop,0,3,5,0));

  filerunnum = new TGNumberEntry(filenamef,0,5,999,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative);
  filerunnum->SetButtonToNum(1);
  filenamef->AddFrame(filerunnum,new TGLayoutHints(kLHintsLeft|kLHintsTop,0,3,4,0));
  filerunnum->Associate(this);

  filesetdone = new TGTextButton(filenamef,"Complete");
  filesetdone->Connect("Pressed()","MainFrame",this,"SetLSFileName()");
  filenamef->AddFrame(filesetdone,new TGLayoutHints(kLHintsLeft|kLHintsTop,0,3,4,0));
  filesetgroup->AddFrame(filenamef,new TGLayoutHints(kLHintsExpandX|kLHintsTop));
  
  TabPanel->AddFrame(filesetgroup,new TGLayoutHints(kLHintsExpandX|kLHintsTop));

  // Control of list mode run
  TGGroupFrame *controlgroup = new TGGroupFrame(TabPanel,"Control");

  // start/stop LSM run button
  TGHorizontalFrame *cgrouphframe=new TGHorizontalFrame(controlgroup);
  startdaq = new TGTextButton(cgrouphframe,"LSRunStart");
  fClient->GetColorByName("red", color);
  startdaq->SetTextColor(color, false);
  startdaq->SetFont("-adobe-helvetica-medium-r-*-*-20-*-*-*-*-*-iso8859-1", false);
  startdaq->Connect("Pressed()","MainFrame",this,"StartLSRun()");
  startdaq->SetEnabled(0);
  startdaq->Resize(110,110);
  startdaq->ChangeOptions(startdaq->GetOptions() | kFixedSize);
  cgrouphframe->AddFrame(startdaq,new TGLayoutHints(kLHintsCenterX|kLHintsTop));
  // send/not send online data stream box
  onlinechk = new TGCheckButton(cgrouphframe,"&Online data");
  fClient->GetColorByName("red", color);
  onlinechk->SetTextColor(color);
  onlinechk->SetState(kButtonDown);
  fonlinedata = 1;
  onlinechk->Connect("Clicked()","MainFrame",this,"SetLSonlinedataf()");
  cgrouphframe->AddFrame(onlinechk,new TGLayoutHints(kLHintsLeft|kLHintsTop,10,4,50,3));

  // save hitogram


  controlgroup->AddFrame(cgrouphframe,new TGLayoutHints(kLHintsExpandX|kLHintsTop));

  TabPanel->AddFrame(controlgroup,new TGLayoutHints(kLHintsExpandX|kLHintsTop));


  
  // restore last run's file information
  char tmp[200];
  ifstream in("../parset/Run.config");
  if(!in.is_open()) return;
  in.getline(tmp,200);
  filepathtext->Insert(tmp);
  in.getline(tmp,200);
  filenametext->Insert(tmp);
  in.close();

  ifstream inrunnumber("../parset/RunNumber");
  if(!inrunnumber.is_open()) return;
  inrunnumber.getline(tmp,200);
  filerunnum->SetText(tmp);
  inrunnumber.close();

  
  // run information
  TGGroupFrame *informationgroup = new TGGroupFrame(TabPanel,"Information");

  
  TGHorizontalFrame *lastruninfor = new TGHorizontalFrame(informationgroup);
  lastruntextinfor = new TGTextEntry(lastruninfor,new TGTextBuffer(30), 10000);
  lastruntextinfor-> SetFont("-adobe-helvetica-bold-r-*-*-14-*-*-*-*-*-iso8859-1", false);
  fClient->GetColorByName("blue", color);
  lastruntextinfor->SetTextColor(color, false);
  lastruntextinfor->SetText(TString::Format("Last run number: %d",int(filerunnum->GetIntNumber())-1).Data());
  lastruntextinfor->Resize(200, 12);
  lastruntextinfor->SetEnabled(kFALSE);
  lastruntextinfor->SetFrameDrawn(kFALSE);
  lastruninfor->AddFrame(lastruntextinfor, new TGLayoutHints(kLHintsLeft | kLHintsTop, 10, 0, 6, 0));
  informationgroup->AddFrame(lastruninfor,new TGLayoutHints(kLHintsExpandX|kLHintsTop));



  
  TabPanel->AddFrame(informationgroup,new TGLayoutHints(kLHintsExpandX|kLHintsTop));



}


void MainFrame::SetLSFileName()
{
  if(detector == NULL){
    cout<<"Modules not booted!"<<endl;
    return ;
  }
  const char *path=filepathtext->GetText();
  const char *filen=filenametext->GetText();
  runnum=(int)filerunnum->GetIntNumber();

  detector->SetRunNumber(runnum);
  for(int i = 0;i < detector->NumModules;i++)
    {
      sprintf(Filename[i],"%s%s_R%04d_M%02d.bin",path,filen,runnum,i);
      sprintf(Histogramname[i],"%s%s_histogram_R%04d_M%02d.bin",path,filen,runnum,i);
      // cout<<i<<" "<<Filename[i]<<endl;
    }

  if(IsDirectoryExists(filepathtext->GetText()))
    {
      ofstream out("../parset/Run.config");
      out<<filepathtext->GetText()<<endl;
      out<<filenametext->GetText()<<endl;
      out.close();
      
      startdaq->SetEnabled(1);
      fstartdaq = 0;
    }
  else
    {
      cout<<"The output file directory does not exist"<<endl;
    }
}

void MainFrame::StartLSRun()
{
  if(fstartdaq == 0)
    {
      onlinemode->SetEnabled(0);
      acquireB->SetEnabled(0);
      saveB->SetEnabled(0);
      filesetdone->SetEnabled(0);
      MenuSetup->DisableEntry(BASE);
      MenuSetup->DisableEntry(ENERGY);
      MenuSetup->DisableEntry(TFILTER);
      MenuSetup->DisableEntry(CFDP);
      MenuSetup->DisableEntry(QDCP);
      MenuSetup->DisableEntry(HISTOGRAM);
      MenuSetup->DisableEntry(SCOPEDT);
      MenuSetup->DisableEntry(FILE_SAVE);
      MenuExpert->DisableEntry(MODVAR);
      MenuExpert->DisableEntry(CSRA);
      MenuExpert->DisableEntry(LOGIC);
      MenuOffline->DisableEntry(OFFLINEADJUSTPAR);
      MenuOffline->DisableEntry(SIMULATION);
      
      SetLSFileName();
      detector->SetOnlineF(fonlinedata);

      for(int i = 0;i < detector->NumModules;i++)
	{
	  cout<<"open: "<<Filename[i]<<endl;
	  if(!detector->OpenSaveFile(i,Filename[i]))
	    {
	      cout<<Filename[i]<<endl;
	      return;
	    }
	}
      if(detector->StartLSMRun(0))
	{
	  cout<<"CANNOT start the LSM Run!"<<endl;

	  MenuSetup->EnableEntry(BASE);
	  MenuSetup->EnableEntry(ENERGY);
	  MenuSetup->EnableEntry(TFILTER);
	  MenuSetup->EnableEntry(CFDP);
	  MenuSetup->EnableEntry(QDCP);
	  MenuSetup->EnableEntry(HISTOGRAM);
	  MenuSetup->EnableEntry(SCOPEDT);
	  MenuSetup->EnableEntry(FILE_SAVE);
	  MenuExpert->EnableEntry(MODVAR);
	  MenuExpert->EnableEntry(CSRA);
	  MenuExpert->EnableEntry(LOGIC);
	  MenuOffline->EnableEntry(OFFLINEADJUSTPAR);
	  MenuOffline->EnableEntry(SIMULATION);
	  acquireB->SetEnabled(1);
	  saveB->SetEnabled(1);
	  onlinemode->SetEnabled(1);
	  filesetdone->SetEnabled(1);
	  
	  return;
	}


      
      usleep(100000); //delay for the DSP boot 
      // sleep(2);// wait 2 seconds for modules to be ready
      // start a new run, not resume
      fstartdaq = 1;
      fstopdaq = 0;
      startdaq->SetText("LSRunStop");
      LSRunReadData();
    }
  else
    {
      fstartdaq = 0;
      startdaq->SetText("LSRunStart");
      filerunnum->SetIntNumber((++runnum));

      ofstream outrunnumber("../parset/RunNumber");
      outrunnumber<<filerunnum->GetIntNumber();
      outrunnumber.close();

      lastruntextinfor->SetText(TString::Format("Last run number: %d",int(filerunnum->GetIntNumber())-1).Data());

      MenuSetup->EnableEntry(BASE);
      MenuSetup->EnableEntry(ENERGY);
      MenuSetup->EnableEntry(TFILTER);
      MenuSetup->EnableEntry(CFDP);
      MenuSetup->EnableEntry(QDCP);
      MenuSetup->EnableEntry(HISTOGRAM);
      MenuSetup->EnableEntry(SCOPEDT);
      MenuSetup->EnableEntry(FILE_SAVE);
      MenuExpert->EnableEntry(MODVAR);
      MenuExpert->EnableEntry(CSRA);
      MenuExpert->EnableEntry(LOGIC);
      MenuOffline->EnableEntry(OFFLINEADJUSTPAR);
      MenuOffline->EnableEntry(SIMULATION);
      acquireB->SetEnabled(1);
      saveB->SetEnabled(1);
      onlinemode->SetEnabled(1);
      filesetdone->SetEnabled(1);
    }
}

void MainFrame::LSRunReadData()
{
  cout<<"MainFrame:: read loop.."<<endl;
  while(fstartdaq){
    detector->ReadDataFromModules(0,0); // during the run
    gSystem->ProcessEvents();
  }
  cout<<"done!!!!!!"<<endl;
  int counter = 0;
  while(detector->StopLSMRun()){
    // failed to stop run 
    sleep(1); // wait 1s then try again
    counter++;
    if(counter>10) break;
  }

  for(int i = 0;i < detector->NumModules;i++)
    {
      detector->SaveHistogram(Histogramname[i],i);
    }
  
  cout<<"finish!"<<endl;
}

void MainFrame::SetOnlineMode()
{
  if(onlinemode->IsOn())
    {
      flagonlinemode = 0;
    }
  else
    {
      flagonlinemode = 1;
    }

  MenuSetup->DisableEntry(BASE);
  MenuSetup->DisableEntry(ENERGY);
  MenuSetup->DisableEntry(TFILTER);
  MenuSetup->DisableEntry(CFDP);
  MenuSetup->DisableEntry(QDCP);
  MenuSetup->DisableEntry(HISTOGRAM);
  MenuSetup->DisableEntry(SCOPEDT);
  MenuSetup->DisableEntry(FILE_SAVE);
  MenuExpert->DisableEntry(MODVAR);
  MenuExpert->DisableEntry(CSRA);
  MenuExpert->DisableEntry(LOGIC);
  MenuOffline->DisableEntry(OFFLINEADJUSTPAR);
  MenuOffline->DisableEntry(SIMULATION);
  bootB->SetEnabled(1);

  fClient->GetColorByName("blue", color);
  StateMsgFold1->SetTextColor(color, false);
  StateMsgFold1->SetText("System not booted");

  acquireB->SetEnabled(0);
  saveB->SetEnabled(0);
  startdaq->SetEnabled(0);
}

void MainFrame::SetLSonlinedataf()
{
  if(onlinechk->IsOn()){
    fonlinedata = 1;
    detector->SetOnlineF(1);
    cout<<"DAQ will send online data!"<<endl;
  }else{
    fonlinedata = 0;
    detector->SetOnlineF(0);
    cout<<"DAQ wont send online data!"<<endl;
  }
}

bool MainFrame::IsDirectoryExists(const char *path)
{
  struct stat fileStat;
  if ((stat(path, &fileStat) == 0) && S_ISDIR(fileStat.st_mode))
    return true;
  else
    return false;
}
