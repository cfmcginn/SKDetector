#include <iostream>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TDatime.h"

#include "include/pseudoTowerGeometry.h"
#include "include/getLinBins.h"

int treeFormat(const std::string inFileName)
{
  TDatime* date = new TDatime();
  TFile* outFile_p = new TFile(("learn_" + std::to_string(date->GetDate()) + ".root").c_str(), "RECREATE");
  TTree* learnTree_p = new TTree("learnTree", "");
  
  pseudoTowGeo pTow;

  std::vector<double> etaTowBounds = pTow.getEtaTowBounds();
  std::vector<int> etaTowNPhi = pTow.getNTowInPhi();

  std::vector<double> towEtaLow;
  std::vector<double> towEtaHi;
  std::vector<double> towPhiLow;
  std::vector<double> towPhiHi;

  const Int_t nPhiTow = 36;
  Double_t phiTow[nPhiTow+1];
  getLinBins(-TMath::Pi(), TMath::Pi(), nPhiTow, phiTow);
  for(Int_t pI = 0; pI < nPhiTow; ++pI){
    towPhiLow.push_back(phiTow[pI]);
    towPhiHi.push_back(phiTow[pI+1]);
  }

  std::cout << etaTowBounds.size() << ", " << etaTowNPhi.size() << std::endl;

  for(unsigned int tI = 0; tI < etaTowNPhi.size(); ++tI){
    if(etaTowNPhi.at(tI) != nPhiTow) continue;

    towEtaLow.push_back(etaTowBounds.at(tI));
    towEtaHi.push_back(etaTowBounds.at(tI+1));
  }

  std::cout << "toweta: " << std::endl;
  for(unsigned int tI = 0; tI < towEtaLow.size(); ++tI){
    std::cout << " " << towEtaLow.at(tI) << "-" << towEtaHi.at(tI) << ", ";
  }
  std::cout << std::endl;

  std::cout << "towphi: " << std::endl;
  for(unsigned int tI = 0; tI < towPhiLow.size(); ++tI){
    std::cout << " " << towPhiLow.at(tI) << "-" << towPhiHi.at(tI) << ", ";
  }
  std::cout << std::endl;

  const Int_t nEtaTow = 2*towEtaLow.size();
  const Int_t nEtaPhiTow = nEtaTow*nPhiTow;
  Float_t etaPhiSum_[nEtaPhiTow];
  Float_t etaCent_[nEtaPhiTow];
  Float_t phiCent_[nEtaPhiTow];

  UInt_t run_, lumi_;
  ULong64_t evt_;
  Int_t hiBin_;
  Float_t evtPlanePhi_;

  learnTree_p->Branch("run", &run_, "run/i");
  learnTree_p->Branch("lumi", &lumi_, "lumi/i");
  learnTree_p->Branch("evt", &evt_, "evt/l");
  learnTree_p->Branch("hiBin", &hiBin_, "hiBin/I");
  learnTree_p->Branch("evtPlanePhi", &evtPlanePhi_, "evtPlanePhi/F");
  learnTree_p->Branch("etaPhiSum", etaPhiSum_, ("etaPhiSum[" + std::to_string(nEtaPhiTow) +"]/F").c_str());
  learnTree_p->Branch("etaCent", etaCent_, ("etaCent[" + std::to_string(nEtaPhiTow) +"]/F").c_str());
  learnTree_p->Branch("phiCent", phiCent_, ("phiCent[" + std::to_string(nEtaPhiTow) +"]/F").c_str());

  TFile* inFile_p = new TFile(inFileName.c_str(), "READ");
  TTree* pfTree_p = (TTree*)inFile_p->Get("pfcandAnalyzer/pfTree");
  TTree* hiTree_p = (TTree*)inFile_p->Get("hiEvtAnalyzer/HiTree");

  std::vector<float>* pfPt_p=0;
  std::vector<float>* pfEta_p=0;
  std::vector<float>* pfPhi_p=0;
  
  const Int_t hiNevtPlaneMax_ = 29;
  Int_t hiNevtPlane_;
  Float_t hiEvtPlanes_[hiNevtPlaneMax_];

  pfTree_p->SetBranchStatus("*", 0);
  pfTree_p->SetBranchStatus("pfPt", 1);
  pfTree_p->SetBranchStatus("pfPhi", 1);
  pfTree_p->SetBranchStatus("pfEta", 1);

  pfTree_p->SetBranchAddress("pfPt", &pfPt_p);
  pfTree_p->SetBranchAddress("pfPhi", &pfPhi_p);
  pfTree_p->SetBranchAddress("pfEta", &pfEta_p);

  hiTree_p->SetBranchStatus("*", 0);
  hiTree_p->SetBranchStatus("run", 1);
  hiTree_p->SetBranchStatus("lumi", 1);
  hiTree_p->SetBranchStatus("evt", 1);
  hiTree_p->SetBranchStatus("hiNevtPlane", 1);
  hiTree_p->SetBranchStatus("hiEvtPlanes", 1);

  hiTree_p->SetBranchAddress("run", &run_);
  hiTree_p->SetBranchAddress("lumi", &lumi_);
  hiTree_p->SetBranchAddress("evt", &evt_);
  hiTree_p->SetBranchAddress("hiNevtPlane", &hiNevtPlane_);
  hiTree_p->SetBranchAddress("hiEvtPlanes", hiEvtPlanes_);

  const Int_t nEntries = pfTree_p->GetEntries();

  std::cout << "Processing..." << std::endl;
  for(Int_t entry = 0; entry < nEntries; ++entry){
    if(entry%10000 == 0) std::cout << " Entry: " << entry << "/" << nEntries << std::endl;

    pfTree_p->GetEntry(entry);
    hiTree_p->GetEntry(entry);

    if(hiNevtPlane_ == 0) continue;
    evtPlanePhi_ = hiEvtPlanes_[8];

  
    for(Int_t i = 0; i < nEtaPhiTow; ++i){
      etaPhiSum_[i] = 0.0;
      etaCent_[i] = -999.;
      phiCent_[i] = -999.;
    }

    for(unsigned int pI = 0; pI < pfPt_p->size(); ++pI){
      Int_t etaPos = -1;
      Int_t phiPos = -1;

      for(unsigned int eI = 0; eI < towEtaLow.size(); ++eI){
	if(pfEta_p->at(pI) >= towEtaLow.at(eI) && pfEta_p->at(pI) < towEtaHi.at(eI)){
	  etaPos = eI;
	  break;
	}
      }

      if(etaPos == -1) continue;

      for(unsigned int eI = 0; eI < towPhiLow.size(); ++eI){
	if(pfPhi_p->at(pI) >= towPhiLow.at(eI) && pfPhi_p->at(pI) < towPhiHi.at(eI)){
	  phiPos = eI;
	  break;
	}
      }
      if(phiPos == -1 && pfPhi_p->at(pI) == towPhiHi.at(towPhiHi.size()-1)) phiPos = towPhiHi.size()-1;

      etaPhiSum_[etaPos*nPhiTow + phiPos] += pfPt_p->at(pI);
      etaCent_[etaPos*nPhiTow + phiPos] = (towEtaLow.at(etaPos) + towEtaHi.at(etaPos))/2.;
      phiCent_[etaPos*nPhiTow + phiPos] = (towPhiLow.at(phiPos) + towPhiHi.at(phiPos))/2.;
    }

    learnTree_p->Fill();
  }

  inFile_p->Close();
  delete inFile_p;

  outFile_p->cd();

  learnTree_p->Write("", TObject::kOverwrite);
  delete learnTree_p;

  outFile_p->Close();
  delete outFile_p;

  delete date;

  return 0;
}

int main(int argc, char* argv[])
{
  if(argc != 2){
    std::cout << "Usage: ./treeFormat.exe <inFileName>" << std::endl;
    return 1;
  }

  int retVal = 0;
  retVal += treeFormat(argv[1]);
  return retVal;
}
