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
  TFile **Files = new TFile*[NumKeys];
  TH2F **PID = new TH2F*[NumKeys];
  TTree **FiltTree = new TTree*[NumKeys];
  TH2F *TDC_vs_IC = new TH2F("TDC_vs_IC","TDC vs Ion Chamber",1000,-1600,-800,1000,0,25000);
  int KeyCounter = 0;
  
  std::string LongName = argv[1];
  std::string ShortName;
  std::size_t found = LongName.find_last_of("/");
  ShortName = LongName.substr(found+1);

  while((CutKey=(TKey*)CutNext())){
    std::cout << " Class Name : " << CutKey->GetClassName() << std::endl; 

    // If you ARE a TCutG go in here
    if(strcmp(CutKey->GetClassName(),"TCutG")){
      CutVect.push_back((TCutG*)c->Get(CutKey->GetName()));
      
      
      Files[KeyCounter] = new TFile(Form("Cut%s_%s",CutKey->GetName(),ShortName.c_str()),"RECREATE");
      PID[KeyCounter] = new TH2F(Form("TDC_vs_IC_%s",CutKey->GetName()),
				 Form("TDC vs Ion Chamber Gated On _%s",CutKey->GetName())
				 ,1000,-1600,-800,1000,0,25000);
      FiltTree[KeyCounter] = new TTree("FiltTree","FiltTree");
      FiltTree[KeyCounter]->Branch("TGretina",&g);
      FiltTree[KeyCounter]->Branch("TS800",&s800);
      FiltTree[KeyCounter]->Branch("TBank29",&b29);
      
      KeyCounter++;
    }
  }
  return 0;
  // Loop over Tree:
  int entrees = EventTree->GetEntries();
  std::cout << "> Looping over : " << entrees << std::endl;
  
  float c1 = 1010;
  float c2 = 0.08;
  /*  std::cout << "> Enter a value for c1: ";
  std::cin >> c1;
  std::cout << "> Enter a value for c2: ";
  std::cin >> c2;
  */
  float pidx = 0;
  float pidy = 0;
  for(int i=0;i<entrees;i++){
    if(i%10000==0){
      printf("> At Event %i of %i  \r",i,entrees);
      fflush(stdout);
    }
    EventTree->GetEntry(i);
    
    pidx = s800->GetTofE1_TDC(c1,c2);
    pidy = s800->GetIonChamber().Charge();

    TDC_vs_IC->Fill(pidx,pidy);
    
    for(int j=0;j<CutVect.size();j++){
      if(CutVect.at(j)->IsInside(pidx,pidy)){
	FiltTree[j]->Fill();
	PID[j]->Fill(pidx,pidy);    
      }
    }
  }

  
  std::cout << std::endl;

  // Write histos and trees.
  for(int k=0;k<CutVect.size();k++){
    Files[k]->cd();
    TDC_vs_IC->Write();
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
