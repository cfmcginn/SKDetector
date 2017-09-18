//c and cpp dependencies
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <time.h>

//ROOT dependencies
#include "TFile.h"
#include "TTree.h"
#include "TDatime.h"
#include "TH2F.h"
#include "TRandom3.h"
#include "TMath.h"

//Local dependencies
#include "include/doGlobalDebug.h"
#include "include/genParticleClass.h"
#include "include/getLinBins.h"
#include "include/etaPhiFunc.h"


void prettyTH2(TH2* inHist_p)
{
  inHist_p->GetXaxis()->CenterTitle();
  inHist_p->GetYaxis()->CenterTitle();

  return;
}

int runGenSK(const std::string inFileName, std::string outFileName = "")
{
  //do a little string handling for convenient filenames
  TDatime* date = new TDatime();

  if(outFileName.size() == 0) outFileName = inFileName;
  while(outFileName.find("/") != std::string::npos){outFileName.replace(0,outFileName.find("/")+1,"");}
  outFileName.replace(outFileName.find(".root"),5,"");
  outFileName = outFileName + "_" + std::to_string(date->GetDate()) + ".root";
  delete date;

  //output file + other necessities
  TFile* outFile_p = new TFile(outFileName.c_str(), "RECREATE");

  //MST periodicity generator for random cones
  TRandom3* randGen_p = new TRandom3(0);
  const Int_t nRC = 11;
  const Double_t absEtaMaxRC = 2.;
  const Double_t rcConeSize = .4;

  TH2F* rhoVsSKCut_p = new TH2F("rhoVsSKCut_h", ";#rho;SK Cut (GeV)", 300, 0, 300, 100, 0, 5);
  prettyTH2(rhoVsSKCut_p);

  TH2F* skRhoVsSKCut_p = new TH2F("skRhoVsSKCut_h", ";#rho_{SK};SK Cut (GeV)", 300, 0, 300, 100, 0, 5);
  prettyTH2(skRhoVsSKCut_p);

  TH2F* rhoVsSKRho_p = new TH2F("rhoVsSKRho_h", ";#rho;#rho_{SK}", 300, 0, 300, 100, 0, 50);
  prettyTH2(rhoVsSKRho_p);

  TH2F* rcRhoVsRCSKRho_p = new TH2F("rcRhoVsRCSKRho_h", ";#rho (All RC);#rho_{SK} (All RC)", 300, 0, 300, 200, 0, 200);
  prettyTH2(rcRhoVsRCSKRho_p);

  TFile* inFile_p = new TFile(inFileName.c_str(), "READ");
  TTree* genTree_p = (TTree*)inFile_p->Get("HiGenParticleAna/hi");

  genParticleClass particles;

  genTree_p->SetBranchStatus("*", 0);
  genTree_p->SetBranchStatus("pt", 1);
  genTree_p->SetBranchStatus("phi", 1);
  genTree_p->SetBranchStatus("eta", 1);
  genTree_p->SetBranchStatus("chg", 1);
  genTree_p->SetBranchStatus("pdg", 1);

  genTree_p->SetBranchAddress("pt", &particles.pt_p);
  genTree_p->SetBranchAddress("phi", &particles.phi_p);
  genTree_p->SetBranchAddress("eta", &particles.eta_p);
  genTree_p->SetBranchAddress("chg", &particles.chg_p);
  genTree_p->SetBranchAddress("pdg", &particles.pdg_p);

  const Int_t nEntries = genTree_p->GetEntries();

  //Construct SK grid
  const Int_t nSKEtaBins = 10;
  const Int_t nSKPhiBins = 16;
  Double_t skEtaBins[nSKEtaBins+1];
  Double_t skPhiBins[nSKPhiBins+1];
  getLinBins(-absEtaMaxRC, absEtaMaxRC, nSKEtaBins, skEtaBins);
  getLinBins(-TMath::Pi(), TMath::Pi(), nSKPhiBins, skPhiBins);

  Double_t maxTrackVal[nSKEtaBins][nSKPhiBins];

  std::cout << "Processing " << nEntries << " events..." << std::endl;

  clock_t t;
  t = clock();
  const double clockPrintInterval = 10;


  for(Int_t entry = 0; entry < nEntries; ++entry){
    if((clock() - t)/CLOCKS_PER_SEC > clockPrintInterval){
      std::cout << " Entry: " << entry << "/" << nEntries << std::endl;
      t = clock();
    }
    genTree_p->GetEntry(entry);

    //clear prev. grid
    for(int i = 0; i < nSKEtaBins; ++i){
      for(int j = 0; j < nSKPhiBins; ++j){maxTrackVal[i][j] = 0;}
    }

    for(int gI = 0; gI < (int)particles.pt_p->size(); ++gI){
      if(TMath::Abs(particles.eta_p->at(gI)) >= absEtaMaxRC) continue;

      Int_t etaPos = -1;
      Int_t phiPos = -1;

      for(int i = 0; i < nSKEtaBins; ++i){
	if(particles.eta_p->at(gI) >= skEtaBins[i] && particles.eta_p->at(gI) < skEtaBins[i+1]){
	  etaPos = i;
	  break;
	}
      }

      for(int i = 0; i < nSKPhiBins; ++i){
	if(particles.phi_p->at(gI) >= skPhiBins[i] && particles.phi_p->at(gI) < skPhiBins[i+1]){
	  phiPos = i;
	  break;
	}
      }

      if(particles.pt_p->at(gI) > maxTrackVal[etaPos][phiPos]) maxTrackVal[etaPos][phiPos] = particles.pt_p->at(gI);      
    }

    std::vector<double> skVect;
    for(int i = 0; i < nSKEtaBins; ++i){
      for(int j = 0; j < nSKPhiBins; ++j){skVect.push_back(maxTrackVal[i][j]);}
    }
    std::sort(std::begin(skVect), std::end(skVect));
    Double_t skCutVal = (skVect.at(skVect.size()/2 - 1) + skVect.at(skVect.size()/2))/2.;

    std::vector<double> ptPastSK_;
    std::vector<double> phiPastSK_;
    std::vector<double> etaPastSK_;
    
    std::vector<double> etaRCVals;
    std::vector<double> phiRCVals;
    std::vector<double> sumRCVals;
    std::vector<double> sumRCSKVals;
    
    while(etaRCVals.size() < nRC){
      Double_t tempEta = randGen_p->Uniform(-absEtaMaxRC, absEtaMaxRC);
      Double_t tempPhi = randGen_p->Uniform(-TMath::Pi(), TMath::Pi());

      Bool_t isGoodVals = true;
      for(unsigned int i = 0; i < etaRCVals.size(); ++i){
	if(getDR(etaRCVals.at(i), phiRCVals.at(i), tempEta, tempPhi) < rcConeSize){
	  isGoodVals = false;
	  break;
	}
      }

      if(!isGoodVals) continue;

      etaRCVals.push_back(tempEta);
      phiRCVals.push_back(tempPhi);
      sumRCVals.push_back(0);
      sumRCSKVals.push_back(0);
    }

    for(int gI = 0; gI < (int)particles.pt_p->size(); ++gI){
      for(unsigned int i = 0; i < etaRCVals.size(); ++i){
	if(getDR(etaRCVals.at(i), phiRCVals.at(i), particles.eta_p->at(gI), particles.phi_p->at(gI)) < rcConeSize){
	  sumRCVals.at(i) += particles.pt_p->at(gI);
	  break;
	}
      }
      
      if(particles.pt_p->at(gI) > skCutVal){
	ptPastSK_.push_back(particles.pt_p->at(gI));
	phiPastSK_.push_back(particles.phi_p->at(gI));
	etaPastSK_.push_back(particles.eta_p->at(gI));
      }
    }

    for(int gI = 0; gI < (int)ptPastSK_.size(); ++gI){
      for(unsigned int i = 0; i < etaRCVals.size(); ++i){
	if(getDR(etaRCVals.at(i), phiRCVals.at(i), etaPastSK_.at(gI), phiPastSK_.at(gI)) < rcConeSize){
	  sumRCSKVals.at(i) += ptPastSK_.at(gI);
	  break;
	}
      }
    } 

    for(int i = 0; i < (int)sumRCVals.size(); ++i){
      rcRhoVsRCSKRho_p->Fill(sumRCVals.at(i)/(TMath::Pi()*rcConeSize*rcConeSize), sumRCSKVals.at(i)/(TMath::Pi()*rcConeSize*rcConeSize));
    }
 
    std::sort(std::begin(sumRCVals), std::end(sumRCVals));
    std::sort(std::begin(sumRCSKVals), std::end(sumRCSKVals));
    Double_t rhoVal = (sumRCVals.at(sumRCVals.size()/2))/(TMath::Pi()*rcConeSize*rcConeSize);
    rhoVsSKCut_p->Fill(rhoVal, skCutVal);

    Double_t skRhoVal = (sumRCSKVals.at(sumRCSKVals.size()/2))/(TMath::Pi()*rcConeSize*rcConeSize);
    skRhoVsSKCut_p->Fill(skRhoVal, skCutVal);

    rhoVsSKRho_p->Fill(rhoVal, skRhoVal);
  }

  inFile_p->Close();
  delete inFile_p;

  outFile_p->cd();

  rhoVsSKCut_p->Write("", TObject::kOverwrite);
  delete rhoVsSKCut_p;

  skRhoVsSKCut_p->Write("", TObject::kOverwrite);
  delete skRhoVsSKCut_p;

  rhoVsSKRho_p->Write("", TObject::kOverwrite);
  delete rhoVsSKRho_p;

  rcRhoVsRCSKRho_p->Write("", TObject::kOverwrite);
  delete rcRhoVsRCSKRho_p;

  outFile_p->Close();
  delete outFile_p;

  return 0;
}

int main(int argc, char* argv[])
{
  if(argc != 2 && argc != 3){
    std::cout << "Usage: ./runGenSK.exe <inFileName> <opt outFileName>" << std::endl;
    return 1;
  }

  int retVal = 0;
  if(argc == 2) retVal += runGenSK(argv[1]);
  else if(argc == 3) retVal += runGenSK(argv[1], argv[2]);

  return retVal;
}
