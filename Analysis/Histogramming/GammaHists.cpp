// g++ GammaHists.cpp `grutinizer-config --root` -o WhateverYouWant
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
  float beta = 0.37;
  //std::cout << "> Enter a value for beta : ";
  //std::cin >> beta;

  
  //========================
  // Open File and get Tree:
  TFile *f = new TFile(argv[1],"READ");
  TTree *FiltTree = (TTree*)f->Get("FiltTree");
  TGretina *g  = 0;
  TS800 *s800  = 0;
  TBank29 *b29 = 0;
  FiltTree->SetBranchAddress("TGretina",&g);
  FiltTree->SetBranchAddress("TS800",&s800);
  FiltTree->SetBranchAddress("TBank29",&b29);

  //========================
  // Open File to Write To:
  TFile *fWrite = new TFile(argv[2],"RECREATE");

  //===================
  // Define Histograms:
  TH2F *Gretina_Summary = new TH2F("Gretina_Summary","Gretina Summary",4000,0,4000,60,20,80);
  TH2F *Dopp_Beta = new TH2F("Dopp_Beta","Doppler vs Beta",2000,0,4000,101,0.2,0.5);

  TH1F *LabFrame = new TH1F("LabFrame","Gamma in Lab Frame",1000,0,4000);
  TH1F *Dopp = new TH1F("Dopp",Form("Doppler Cor Value %f",beta),1000,0,4000);

  //================
  // Loop over Tree:
  int entrees = FiltTree->GetEntries();
  std::cout << "> Looping over : " << entrees << std::endl;
  float dummy_beta = 0.2;
  for(int i=0;i<entrees;i++){
    if(i%1000==0){
      printf("> At Event %i of %i  \r",i,entrees);
      fflush(stdout);
    }
    FiltTree->GetEntry(i);
    for(int j=0;j<g->Size();j++){
      TGretinaHit hit = g->GetGretinaHit(j);
      dummy_beta = 0.2;

      //=================
      // Fill Histograms:
      Gretina_Summary->Fill(hit.GetCoreEnergy(),hit.GetCrystalId());
      LabFrame->Fill(hit.GetCoreEnergy());
      Dopp->Fill(hit.GetDoppler(beta));

      for(int z=0;z<100;z++){
	dummy_beta += 0.3/100.0;
	Dopp_Beta->Fill(hit.GetDoppler(dummy_beta),dummy_beta);
      }
    }
 

  }

  
  std::cout << std::endl;

  //==================
  // Write Histograms:
  Gretina_Summary->Write();
  LabFrame->Write();
  Dopp->Write();
  Dopp_Beta->Write();
  // Close Files:
  fWrite->Close();
  f->Close();
  
  return 0;

}
