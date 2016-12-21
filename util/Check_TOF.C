//std::vector<GH1D*> Proj_vect;

TList *CheckTOF_vs_Time(double lineX1=0,double lineX2=6000,double lineY1=-1050,double lineY2=-1050){

  // Declare stuff
  TList *HistList = new TList;
  TList *FileList = gROOT->GetListOfFiles();
  int NumFiles = FileList->GetSize();
  TLegend *leg = new TLegend(0.2,0.2,0.5,0.5);
  TLegend *leg2 = new TLegend(0.2,0.2,0.5,0.5);
  TCanvas *TwoDCanvas = new TCanvas("TwoDCanvas","2D Canvas TOF vs S800 Timestamp");
  TCanvas *ProjCanvas = new TCanvas("ProjCanvas","1D TOF Projection");
  TCanvas *StackCanvas = new TCanvas("StackCanvas","1D TOF Projection Stacked");
  TCanvas *ScaledCanv = new TCanvas("ScaledCanv","Scaled Canvas");

  // Grab the run number:
  if(NumFiles%4==0){
    int FirstDivide = NumFiles/4;
    TwoDCanvas->Divide(FirstDivide,4);
    ProjCanvas->Divide(FirstDivide,4);
  }else if(NumFiles%3==0 && NumFiles%4!=0){ 
    int FirstDivide = NumFiles/3;
    TwoDCanvas->Divide(FirstDivide,3);
    ProjCanvas->Divide(FirstDivide,3);
  }else if(NumFiles%2==0 && NumFiles%4!=0 && NumFiles%3!=0){
    int FirstDivide = NumFiles/2;
    TwoDCanvas->Divide(FirstDivide,2);
    ProjCanvas->Divide(FirstDivide,2);
  }else{
    TwoDCanvas->Divide(NumFiles);
    ProjCanvas->Divide(NumFiles);
  }

  // begin loop over files:
  for(int i=0;i<NumFiles;i++){
    TFile *WorkingFile = (TFile*)FileList->At(i);
    string FileName = WorkingFile->GetName();
    cout << " File Name = " << FileName << endl;
    size_t found = FileName.find_last_of(".");
    string run = FileName.substr(found-4,4);
    stringstream runSS;
    double runNumb =0;
    runSS << run;
    runSS >> runNumb;    
    if(runNumb==0){
      run = FileName.substr(found-3,3);
      stringstream runSS2;
      runSS2 << run;
      runSS2 >> runNumb;
    }

    if(runNumb==0){
      run = FileName.substr(found-2,2);
      stringstream runSS3;
      runSS3 << run;
      runSS3 >> runNumb;
    }

    if(runNumb==0){
      run = FileName.substr(found-1,1);
      stringstream runSS3;
      runSS3 << run;
      runSS3 >> runNumb;
    }

    
    
    WorkingFile->cd("CorrelatedMESY");
    TwoDCanvas->cd(i+1);

    TOF_vs_S800Timestamp->SetTitle(Form("Corrected OBJ TOF vs S800 Timestamp Run %s",run.c_str()));
    TOF_vs_S800Timestamp->GetYaxis()->SetRangeUser(-1050,-900);
    TOF_vs_S800Timestamp->Draw();

    TLine *line1 = new TLine(lineX1,lineY1,lineX2,lineY2);
    line1->SetLineStyle(kDashed);
    line1->SetLineWidth(3);
    line1->Draw();

    GH1D *Proj1 = TOF_vs_S800Timestamp->ProjectionY(Form("Corrected OBJ TOF Run %s",run.c_str()));
    Proj1->SetTitle(Form("Corrected OBJ TOF Run %s",run.c_str()));
    GH1D *Proj_Clone = (GH1D*)Proj1->Clone();
    GH1D *Proj_Clone2 = (GH1D*)Proj1->Clone();

    double Proj1_yMax = Proj1->GetMaximum();
    ProjCanvas->cd(i+1);
    Proj1->Draw();
    StackCanvas->cd();

    Proj_Clone->SetLineColor(i+1);
    Proj_Clone->SetTitle("");
    leg->AddEntry(Proj_Clone,Form("Run%s",run.c_str()),"l");
    if(i==0)
      Proj_Clone->Draw();
    else
      Proj_Clone->Draw("same");

    //    GH1D *Proj_Clone  = (GH1D*)Proj1->Clone();
    if(i<4){
      Proj_Clone2->SetLineColor(i%4+1);
      
    }else if(i>3 && i<8){
      Proj_Clone2->SetLineColor(i%4+1);
      Proj_Clone2->SetLineStyle(2);
    }else if(i>7 && i<12){
      Proj_Clone2->SetLineColor(i%4+1);
      Proj_Clone2->SetLineStyle(3);
    }else{
      Proj_Clone2->SetLineColor(i%4+1);
      Proj_Clone2->SetLineStyle(4);
    }
    
    ScaledCanv->cd();
    
    Proj_Clone2->Scale(1.0/Proj_Clone2->Integral());
    if(i==0){
      Proj_Clone2->SetLineColor(1);
      Proj_Clone2->Draw();
    }else{
      Proj_Clone2->SetLineColor(2);
      Proj_Clone2->Draw("same");
    }
    
    leg2->AddEntry(Proj_Clone2,Form("Run%s",run.c_str()),"l");
  }
  
  
  StackCanvas->cd();
  leg->Draw("same");
  ScaledCanv->cd();
  leg2->Draw("same");
}



TList *FitTOF_vs_Time(double LowFit,double HighFit,
		      double InitVal = 0, string Dir){
  TList *HistList = new TList;
  TList *FileList = gROOT->GetListOfFiles();
  int NumFiles = FileList->GetSize();
  
  TCanvas *ProjCanvas = new TCanvas("ProjCanvas","1D TOF Projection");
  if(NumFiles%4==0){
    int FirstDivide = NumFiles/4;
    ProjCanvas->Divide(FirstDivide,4);
  }else if(NumFiles%3==0 && NumFiles%4!=0){ 
    int FirstDivide = NumFiles/3;
    ProjCanvas->Divide(FirstDivide,3);
  }else if(NumFiles%2==0 && NumFiles%4!=0 && NumFiles%3!=0){
    int FirstDivide = NumFiles/2;
    ProjCanvas->Divide(FirstDivide,2);
  }else{
    ProjCanvas->Divide(NumFiles);
  }

  for(int i=0;i<NumFiles;i++){
  
    TFile *WorkingFile = (TFile*)FileList->At(i);

    string FileName = WorkingFile->GetName();
    cout << " File Name = " << FileName << endl;
    size_t found = FileName.find_last_of(".");
    string run = FileName.substr(found-4,4);
    stringstream runSS;
    double runNumb =0;
    runSS << run;
    runSS >> runNumb;    
    if(runNumb==0){
      run = FileName.substr(found-3,3);
      stringstream runSS2;
      runSS2 << run;
      runSS2 >> runNumb;
    }

    if(runNumb==0){
      run = FileName.substr(found-2,2);
      stringstream runSS3;
      runSS3 << run;
      runSS3 >> runNumb;
    }

    if(runNumb==0){
      run = FileName.substr(found-1,1);
      stringstream runSS3;
      runSS3 << run;
      runSS3 >> runNumb;
    }

    ifstream inFile;
    ofstream outFile;
    string inFileName  = Form("%s/values_run0%s_WithATAOff.val",Dir.c_str(),run.c_str());
    string outFileName = Form("%s/values_run0%s_PIDOffset.val",Dir.c_str(),run.c_str());

    std::cout << " InFile  = " << inFileName << std::endl;
    std::cout << " OutFile = " << outFileName << std::endl;
    inFile.open(inFileName.c_str());
    if(inFile.good()){
      outFile.open(outFileName.c_str());
      while(!(inFile.eof())){
	string eat;
	getline(inFile,eat);
	outFile << eat << std::endl;
      }
    } else{
      inFile.close();
      outFile.open(outFileName.c_str());
    }





    
    WorkingFile->cd("CorrelatedMESY");
    TOF_vs_S800Timestamp->GetYaxis()->SetRangeUser(-1100,-1000);
    GH1D *Proj1 = TOF_vs_S800Timestamp->ProjectionY(Form("Corrected OBJ TOF Run %s",run.c_str()));
    Proj1->SetTitle(Form("Corrected OBJ TOF Run %s",run.c_str()));
    ProjCanvas->cd(i+1);
    Proj1->Draw();
    GGaus *g = GausFit(Proj1,LowFit,HighFit);
    double Cent = g->GetCentroid();
    double PIDOffset = InitVal - Cent;
    cout << " --- Run : " << runNumb << endl;
    cout << " Peak Fit   = " << Cent << endl;
    cout << " Init Val   = " << InitVal << endl;
    cout << " PID Offset = " << PIDOffset << endl;
    outFile << "OBJ_MTOF_OFFSET {" << endl;
    outFile << "  Value: " << PIDOffset << endl;
    outFile << "}" << endl << endl;
    outFile.close();

  }
}
