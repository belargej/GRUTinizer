// g++ SimpleLoop1.cpp `grutinizer-config --root` -o WhateverYouWant
#include <Globals.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>

//#include <TRawFile.h>
//#include <TNSCLEvent.h>
#include <TGretina.h>
#include <TS800.h>
#include <TBank29.h>

#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TCutG.h>
#include <TDirectory.h>
#include <TKey.h>
#include <TList.h>
#include <TCollection.h>


int main(int argc, char** argv){


  if(argc!=3){
    std::cout << " *** USAGE : <prog> <fileIn> <cutFile>" << std::endl;
    return 0;
  }
  
  
  // Open File containing data
  TFile *f = new TFile(argv[1],"READ");

  // Get Tree
  TTree *EventTree = (TTree*)f->Get("EventTree");

  // Set branch addresses
  TGretina *g  = 0;
  TS800 *s800  = 0;
  TBank29 *b29 = 0;
  TGretina *myg  = new TGretina();
  EventTree->SetBranchAddress("TGretina",&g);
  EventTree->SetBranchAddress("TS800",&s800);
  EventTree->SetBranchAddress("TBank29",&b29);

  // Open File containing Cuts:
  TFile *c = new TFile(argv[2],"READ");
  int NumKeys = c->GetNkeys();
  TList *lKeys = c->GetListOfKeys();
  TIter CutNext(lKeys);
  TKey *CutKey;
  
  // Define Cuts, Files, Histograms, etc:
  std::vector<TCutG*> CutVect;
  std::vector<TCutG*> TimeCVect;
  TFile **Files = new TFile*[NumKeys];
  TH2F **PID = new TH2F*[NumKeys];
  TTree **FiltTree = new TTree*[NumKeys];
  TH2F *TDC_vs_IC = new TH2F("TDC_vs_IC","TDC vs Ion Chamber",1000,-1600,-800,1000,0,25000);
  TH2F *Gret_vs_B29T = new TH2F("Gret_vs_B29T","Gretina Energy vs Bank 29 Time",1200,-600,600,2000,0,4000);
  int KeyCounter = 0;
  
  std::string LongName = argv[1];
  std::string ShortName;
  std::size_t found = LongName.find_last_of("/");
  ShortName = LongName.substr(found+1);

  while((CutKey=(TKey*)CutNext())){
    // If you ARE a TCutG go in here
    if(strcmp(CutKey->GetClassName(),"TCutG")==0){
      CutVect.push_back((TCutG*)c->Get(CutKey->GetName()));
      
      
      Files[KeyCounter] = new TFile(Form("Cut%s_%s",CutKey->GetName(),ShortName.c_str()),"RECREATE");
      PID[KeyCounter] = new TH2F(Form("TDC_vs_IC_%s",CutKey->GetName()),
				 Form("TDC vs Ion Chamber Gated On _%s",CutKey->GetName())
				 ,1000,-1600,-800,1000,0,25000);
      FiltTree[KeyCounter] = new TTree("FiltTree","FiltTree");
      FiltTree[KeyCounter]->Branch("TGretina",&myg); // This is not g bc we need to pick and choose!!
      FiltTree[KeyCounter]->Branch("TS800",&s800);
      FiltTree[KeyCounter]->Branch("TBank29",&b29);
      
      KeyCounter++;
    }
    
    if(strcmp(CutKey->GetClassName(),"TDirectoryFile")==0){
      TDirectory *TCDir = (TDirectory*)c->Get(CutKey->GetName());
      TList *lTimeDir = TCDir->GetListOfKeys();
      int NumTimeDirKeys = TCDir->GetNkeys();
      TKey *TimeKey = 0;
      
      c->cd(CutKey->GetName());
      TIter TCNext(lTimeDir);
      while(TimeKey=(TKey*)TCNext()){
	TimeCVect.push_back((TCutG*)TCDir->Get(TimeKey->GetName()));
      }
      c->cd();
    }
  }
  
  /*  std::cout << " Number of time cuts : " << TimeCVect.size() << std::endl;
  for(int j_i = 0;j_i<TimeCVect.size();j_i++){
    std::cout << " HERE " << std::endl;
    std::cout << "Name : " << TimeCVect.at(j_i)->GetName() << std::endl;
    }
  return 0;*/
  // Loop over Tree:
  int entrees = EventTree->GetEntries();
  std::cout << "> Looping over : " << entrees << std::endl;
  
  float c1 = 1010;
  float c2 = 0.08;
  float pidx = 0;
  float pidy = 0;
  float Tx = 0;
  float Ty = 0;
  for(int i=0;i<entrees;i++){
    if(i%10000==0){
      printf("> At Event %i of %i  \r",i,entrees);
      fflush(stdout);
    }
    EventTree->GetEntry(i);
    
    pidx = s800->GetTofE1_TDC(c1,c2);
    pidy = s800->GetIonChamber().Charge();
    TDC_vs_IC->Fill(pidx,pidy); // this gets filled once.
    
    //---------------------------------------------------
    // Here we loop over the PID cuts, and get the index 
    // for the CutVect that the event lives in.  This 
    // is good 
    int mypid = -1;
    for(int j=0;j<CutVect.size();j++){
      if(CutVect.at(j)->IsInside(pidx,pidy)){ // do this cut first, more restrictive.
	mypid = j;
        break;
      }
    }
    
    if(mypid<0)
      continue;
    
    myg->Clear();
    bool time_passed = false;
    int  time_gate = -1;
    for(int k_1 = 0;k_1<g->Size();k_1++){
      TGretinaHit hit = g->GetGretinaHit(k_1);
      Tx = b29->Timestamp()-hit.GetTimestamp();
      Ty = hit.GetCoreEnergy();
      

      for(int k=0;k<TimeCVect.size();k++){
	if(TimeCVect.at(k)->IsInside(Tx,Ty)){
          time_passed = true;
          time_gate = k;
	  Gret_vs_B29T->Fill(Tx,Ty);
          break;
        }
      }
      if(!time_passed)
        continue;

      myg->InsertHit(hit);
    }
    if(myg->Size()<1 || time_gate<0)
      continue;
    FiltTree[mypid]->Fill();
    PID[mypid]->Fill(pidx,pidy);
  }
  
  std::cout << std::endl;

  // Write histos and trees.
  for(int k=0;k<CutVect.size();k++){
    Files[k]->cd();
    TDC_vs_IC->Write();
    Gret_vs_B29T->Write();
    PID[k]->Write();
    FiltTree[k]->Write();
    Files[k]->Close();
  }

  delete[] Files;
  delete[] PID;
  delete[] FiltTree;
  Files = 0;
  PID = 0;
  FiltTree = 0;
      
  
  c->Close();
  f->Close();
  
  return 0;

}
