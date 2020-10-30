#include <Riostream.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <TCanvas.h>
#include "TFile.h"
#include "TTree.h"
#include <iomanip>
//#pragma hdrstop
#include<stdio.h>
#include<stdlib.h>
#include <TH2.h>

void atc_read_runsmonitor()
{
  int i=23071;
//  int i=26071;

  TFile *f1 = TFile::Open(TString::Format("/space/users/todyshev/RunsMonitor/R%02d.root", i).Data());   //текущий заход
  if (f1 == 0) {
      // if we cannot open the file, print an error message and return immediatly
      cout<<"Cannot open root file"<<TString::Format("/space/users/todyshev/RunsMonitor/R%02d.root", i).Data()<<" - file does not exist"<<endl;
      return;
  }
  TDirectory* D1=(TDirectory*)f1->Get("ATC");
  TH1F *h1 = (TH1F*)D1->Get("OATCCountH");
  h1->Draw();

  TFile *f2=new TFile ("/space/users/todyshev/RunsMonitor/R22681.root");        //референсный заход
  TDirectory* D2=(TDirectory*)f2->Get("ATC");
  TH1F *h2 = (TH1F*)D2->Get("OATCCountH");                                             //читаю гистограмму из директории
  h2->SetLineColor(2);
  h2->Draw("same");

  int Nxbin1=h1->GetNbinsX();

  cout<<"Run="<<i<<"\t"<<"Ref. run=22681"<<endl;
  cout<<"Nxbin1="<<Nxbin1<<endl;
  int k1;
  float bin1;
  float bin2;
  float ratio;

  TH1F *h3 = new TH1F("h3","ATC:Normalized occupancy",160,0.5,160.5);

  for(int k1=1; k1<=Nxbin1; k1++)
  {
      bin1=h1->GetBinContent(k1);
      bin2=h2->GetBinContent(k1);
      if(bin2==0) { cout<<"Bad channel:"<<k1<<"\t"<<bin1<<"\t"<<bin2<<endl;
      ratio=0;
      }
      else {
      ratio=bin1/bin2;                                    //соотношение текущего захода к референсному в канале
        if(ratio<=0.3){ cout<<"Bad channel:"<<k1<<"\t"<<bin1<<"\t"<<bin2<<"\t"<<ratio<<endl; }
//	else { cout<<k1<<"\t"<<bin1<<"\t"<<bin2<<"\t"<<ratio<<endl; }
      }
      h3->Fill(k1,ratio);
  }
 // h3->Draw();
}

