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
  TH2F *TDC_vs_IC[5][6];
  TH2F *TDC_vs_AFP[5][6];
  TH2F *TDC_vs_X[5][6];
  
  float c1 = 1000;
  float c2 = 0.07;
  for(int j=0;j<5;j++){
    c2=0.07;
    for(int k=0;k<6;k++){
      TDC_vs_IC[j][k] = new TH2F(Form("TDC_vs_IC_%f_%f",c1,c2),Form("TDC vs Ion Chamber c1=%f c2=%f",c1,c2),1000,-1600,-800,1000,0,25000);
      TDC_vs_AFP[j][k] = new TH2F(Form("TDC_vs_AFP_%f_%f",c1,c2),Form("TDC vs AFP c1=%f c2=%f",c1,c2),1000,-1600,-800,600,-0.1,0.1);
      TDC_vs_X[j][k] = new TH2F(Form("TDC_vs_X_%f_%f",c1,c2),Form("TDC vs Ion Dispersive X c1=%f c2=%f",c1,c2),1000,-1600,-800,600,-300,300);
      c2+=0.01;
    }
    c1+=10;
  }

  
  // Loop over Tree?
  int entrees = EventTree->GetEntries();
  std::cout << "> Looping over : " << entrees << std::endl;
  
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
  
    c1 = 1000;
    for(int j=0;j<5;j++){
      c2 = 0.07;
      for(int k=0;k<6;k++){
	TDC_vs_X[j][k]->Fill(s800->GetTofE1_TDC(c1,c2),s800->GetCrdc(0).GetDispersiveX());
	TDC_vs_IC[j][k]->Fill(s800->GetTofE1_TDC(c1,c2),s800->GetIonChamber().Charge());
	TDC_vs_AFP[j][k]->Fill(s800->GetTofE1_TDC(c1,c2),s800->GetAFP());
	c2+=0.01;
      }
      c1+=10;
    }
 
    //TDC_vs_IC->Fill(s800->GetTofE1_TDC(c1,c2),s800->GetIonChamber().Charge());
    //TDC_vs_X->Fill(s800->GetTofE1_TDC(c1,c2),s800->GetCrdc(0).GetDispersiveX());
    //TDC_vs_AFP->Fill(s800->GetTofE1_TDC(c1,c2),s800->GetAFP());

  }

  
  std::cout << std::endl;


  // Write Histograms:
  //TDC_vs_IC->Write();
  //TDC_vs_X->Write();
  //TDC_vs_AFP->Write();
  for(int j=0;j<5;j++){
    for(int k=0;k<6;k++){
      TDC_vs_X[j][k]->Write();
      TDC_vs_IC[j][k]->Write();
      TDC_vs_AFP[j][k]->Write();
    }
  }
  /* for(int j=0;j<10;j++){
    for(int k=0;k<10;k++){
      TDC_vs_X[j][k];
      TDC_vs_IC[j][k];
      TDC_vs_AFP[j][k];
    }
    }*/
  
  
  // Close Files:
  fWrite->Close();
  f->Close();
  
  return 0;

}
