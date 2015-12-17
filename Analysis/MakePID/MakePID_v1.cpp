// g++ SimpleLoop1.cpp `grutinizer-config --root` -o WhateverYouWant
#include <Globals.h>
#include <iostream>

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


int main(int argc, char** argv){


  if(argc!=3){
    std::cout << " *** USAGE : <prog> <fileIn> <fileOut>" << std::endl;
    return 0;
  }
  

  // Open File
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

  // Open File to Write To
  TFile *fWrite = new TFile(argv[2],"RECREATE");

  // Define Histograms:
  TH2F *TDC_vs_IC = new TH2F("TDC_vs_IC","TDC vs Ion Chamber",1000,-1600,-800,1000,0,25000);
  TH2F *TDC_vs_AFP = new TH2F("TDC_vs_AFP","TDC vs AFP",1000,-1600,-800,600,-0.1,0.1);
  TH2F *TDC_vs_X = new TH2F("TDC_vs_X","TDC vs Dispersive X",1000,-1600,-800,600,-300,300);
  TH2F *Gret_vs_B29T = new TH2F("Gret_vs_B29T","Gretina Energy vs Bank 29 Time",1200,-600,600,2000,0,4000);
  TH2F *Gret_vs_S800T = new TH2F("Gret_vs_S800T","Gretina Energy vs S800 Time",1200,-600,600,2000,0,4000);
  TH2F *Gret0_vs_S800T = new TH2F("Gret0_vs_S800T","Gretina Energy vs S800 Time (T0)",1200,-600,600,2000,0,4000);
  
  

  
  // Loop over Tree?
  int entrees = EventTree->GetEntries();
  std::cout << "> Looping over : " << entrees << std::endl;
  
  float c1 = 1010;
  float c2 = 0.08;
  /* std::cout << "> Enter a value for c1: ";
  std::cin >> c1;
  std::cout << "> Enter a value for c2: ";
  std::cin >> c2;*/
  for(int i=0;i<entrees;i++){
    if(i%10000==0){
      printf("> At Event %i of %i  \r",i,entrees);
      fflush(stdout);
    }
    EventTree->GetEntry(i);
    
    TDC_vs_IC->Fill(s800->GetTofE1_TDC(c1,c2),s800->GetIonChamber().Charge());
    TDC_vs_X->Fill(s800->GetTofE1_TDC(c1,c2),s800->GetCrdc(0).GetDispersiveX());
    TDC_vs_AFP->Fill(s800->GetTofE1_TDC(c1,c2),s800->GetAFP());
    
    for(int k=0;k<g->Size();k++){
      TGretinaHit hit = g->GetGretinaHit(k);
      // Should probably use GetTime(), has walk correction.
      Gret_vs_B29T->Fill(b29->Timestamp()-hit.GetTimestamp(),hit.GetCoreEnergy());
      Gret_vs_S800T->Fill(s800->GetTimestamp()-hit.GetTimestamp(),hit.GetCoreEnergy());
      Gret0_vs_S800T->Fill(s800->GetTimestamp()-hit.GetTime(),hit.GetCoreEnergy());
    }
  }

  
  std::cout << std::endl;


  // Write Histograms:
  TDC_vs_IC->Write();
  TDC_vs_X->Write();
  TDC_vs_AFP->Write();
  Gret_vs_B29T->Write();
  Gret_vs_S800T->Write();
  Gret0_vs_S800T->Write();
  
  // Close Files:
  fWrite->Close();
  f->Close();
  
  return 0;

}
