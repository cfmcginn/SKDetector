#include <iostream>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TDatime.h"
#include "TMath.h"

#include "include/pseudoTowerGeometry.h"
#include "include/getLinBins.h"

int treeFormat(const std::string inFileName, const bool doJetStyle = false)
{
  TDatime* date = new TDatime();
  TFile* outFile_p = new TFile(("learn_" + std::to_string(date->GetDate()) + ".root").c_str(), "RECREATE");
  TTree* learnTree_p = new TTree("learnTree", "");
  
  pseudoTowGeo pTow;

  std::vector<double> etaTowBounds = pTow.getEtaTowBounds();
  std::vector<int> etaTowNPhi = pTow.getNTowInPhi();
  const Int_t nPhiTow72 = 72;
  const Int_t nPhiTow36 = 36;
  const Int_t nPhiTow18 = 18;
  Double_t phiTow72[nPhiTow72+1];
  Double_t phiTow36[nPhiTow36+1];
  Double_t phiTow18[nPhiTow18+1];
  getLinBins(-TMath::Pi(), TMath::Pi(), nPhiTow72, phiTow72);
  getLinBins(-TMath::Pi(), TMath::Pi(), nPhiTow36, phiTow36);
  getLinBins(-TMath::Pi(), TMath::Pi(), nPhiTow18, phiTow18);

  std::vector<double> towEtaCentGlobal;

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
    towEtaCentGlobal.push_back((etaTowBounds.at(tI) + etaTowBounds.at(tI+1))/2.);

    if(etaTowNPhi.at(tI) > 36) continue;

    if(TMath::Abs(etaTowBounds.at(tI)) <= 3 && TMath::Abs(etaTowBounds.at(tI+1)) <= 3) continue;

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

  const Int_t nEtaTow = towEtaLow.size();
  const Int_t nEtaPhiTow = nEtaTow*nPhiTow;
  const Int_t nEtaPhiTowCollapse = 2*nPhiTow;
  Float_t etaPhiSum_[nEtaPhiTow];
  Float_t etaPhiSum_Collapse_[nEtaPhiTowCollapse];
  Float_t etaCent_[nEtaPhiTow];
  Float_t phiCent_[nEtaPhiTow];

  UInt_t run_, lumi_;
  ULong64_t evt_;
  Float_t vz_;
  Int_t hiBin_;
  Float_t evtPlanePhi_;

  Float_t jtptOut_ = -999;
  Float_t rawptOut_ = -999;
  Float_t jtphiOut_ = -999;
  Float_t jtetaOut_ = -999;
  Float_t refptOut_ = -999;

  const Int_t nJtPixels1D = 11;
  const Int_t nJtPixels = nJtPixels1D*nJtPixels1D;
  Float_t jtPixels[nJtPixels+1];

  learnTree_p->Branch("run", &run_, "run/i");
  learnTree_p->Branch("lumi", &lumi_, "lumi/i");
  learnTree_p->Branch("evt", &evt_, "evt/l");
  learnTree_p->Branch("vz", &vz_, "vz/F");
  learnTree_p->Branch("hiBin", &hiBin_, "hiBin/I");
  learnTree_p->Branch("evtPlanePhi", &evtPlanePhi_, "evtPlanePhi/F");
  learnTree_p->Branch("etaPhiSum", etaPhiSum_, ("etaPhiSum[" + std::to_string(nEtaPhiTow) +"]/F").c_str());
  for(Int_t bI = 0; bI < nEtaPhiTowCollapse; ++bI){
    learnTree_p->Branch(("etaPhiSum_Collapse" + std::to_string(bI)).c_str(), &(etaPhiSum_Collapse_[bI]), ("etaPhiSum_Collapse" + std::to_string(bI) + "/F").c_str());
  }
  learnTree_p->Branch("etaCent", etaCent_, ("etaCent[" + std::to_string(nEtaPhiTow) +"]/F").c_str());
  learnTree_p->Branch("phiCent", phiCent_, ("phiCent[" + std::to_string(nEtaPhiTow) +"]/F").c_str());

  if(doJetStyle){
    learnTree_p->Branch("jtpt", &jtptOut_, "jtpt/F");
    learnTree_p->Branch("rawpt", &rawptOut_, "rawpt/F");
    learnTree_p->Branch("jtphi", &jtphiOut_, "jtphi/F");
    learnTree_p->Branch("jteta", &jtetaOut_, "jteta/F");
    learnTree_p->Branch("refpt", &refptOut_, "refpt/F");

    for(Int_t jI = 0; jI < nJtPixels; ++jI){
      learnTree_p->Branch(("jtPixel" + std::to_string(jI)).c_str(), &jtPixels[jI], ("jtPixel" + std::to_string(jI) + "/F").c_str());
    }
  }

  TFile* inFile_p = new TFile(inFileName.c_str(), "READ");
  TTree* pfTree_p = (TTree*)inFile_p->Get("pfcandAnalyzer/pfTree");
  TTree* hiTree_p = (TTree*)inFile_p->Get("hiEvtAnalyzer/HiTree");
  TTree* jetTree_p = 0;
  if(doJetStyle) jetTree_p = (TTree*)inFile_p->Get("akPu4PFJetAnalyzer/t");

  std::vector<float>* pfPt_p=0;
  std::vector<float>* pfEta_p=0;
  std::vector<float>* pfPhi_p=0;
  
  const Int_t hiNevtPlaneMax_ = 29;
  Int_t hiNevtPlane_;
  Float_t hiEvtPlanes_[hiNevtPlaneMax_];

  const Int_t nMaxJet = 500;
  Int_t nref_;
  Float_t jtpt_[nMaxJet];
  Float_t rawpt_[nMaxJet];
  Float_t jteta_[nMaxJet];
  Float_t jtphi_[nMaxJet];
  Float_t refpt_[nMaxJet];

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
  hiTree_p->SetBranchStatus("hiBin", 1);
  hiTree_p->SetBranchStatus("vz", 1);
  hiTree_p->SetBranchStatus("hiNevtPlane", 1);
  hiTree_p->SetBranchStatus("hiEvtPlanes", 1);

  hiTree_p->SetBranchAddress("run", &run_);
  hiTree_p->SetBranchAddress("lumi", &lumi_);
  hiTree_p->SetBranchAddress("evt", &evt_);
  hiTree_p->SetBranchAddress("hiBin", &hiBin_);
  hiTree_p->SetBranchAddress("vz", &vz_);
  hiTree_p->SetBranchAddress("hiNevtPlane", &hiNevtPlane_);
  hiTree_p->SetBranchAddress("hiEvtPlanes", hiEvtPlanes_);

  if(doJetStyle){
    jetTree_p->SetBranchStatus("*", 0);
    jetTree_p->SetBranchStatus("nref", 1);
    jetTree_p->SetBranchStatus("jtpt", 1);
    jetTree_p->SetBranchStatus("rawpt", 1);
    jetTree_p->SetBranchStatus("jtphi", 1);
    jetTree_p->SetBranchStatus("jteta", 1);
    jetTree_p->SetBranchStatus("refpt", 1);

    jetTree_p->SetBranchAddress("nref", &nref_);
    jetTree_p->SetBranchAddress("jtpt", jtpt_);
    jetTree_p->SetBranchAddress("rawpt", rawpt_);
    jetTree_p->SetBranchAddress("jtphi", jtphi_);
    jetTree_p->SetBranchAddress("jteta", jteta_);
    jetTree_p->SetBranchAddress("refpt", refpt_);
  }

  const Int_t nEntries = pfTree_p->GetEntries();

  std::cout << "Processing..." << std::endl;
  for(Int_t entry = 0; entry < nEntries; ++entry){
    if(entry%10000 == 0) std::cout << " Entry: " << entry << "/" << nEntries << std::endl;

    pfTree_p->GetEntry(entry);
    hiTree_p->GetEntry(entry);
    if(doJetStyle) jetTree_p->GetEntry(entry);

    if(hiNevtPlane_ == 0) continue;
    evtPlanePhi_ = hiEvtPlanes_[8];
  
    for(Int_t i = 0; i < nEtaPhiTow; ++i){
      etaPhiSum_[i] = 0.0;
      etaCent_[i] = -999.;
      phiCent_[i] = -999.;
    }

    jtptOut_ = -999;
    rawptOut_ = -999;
    jtphiOut_ = -999;
    jtetaOut_ = -999;
    refptOut_ = -999;

    for(Int_t i = 0; i < nJtPixels; ++i){jtPixels[i] = 0.0;}
    
    for(Int_t i = 0; i < nEtaPhiTowCollapse; ++i){etaPhiSum_Collapse_[i] = 0.0;}

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

      Int_t etaPosCollapse = 0;
      if(pfEta_p->at(pI) > 0) etaPosCollapse = 1;

      for(unsigned int eI = 0; eI < towPhiLow.size(); ++eI){
	if(pfPhi_p->at(pI) >= towPhiLow.at(eI) && pfPhi_p->at(pI) < towPhiHi.at(eI)){
	  phiPos = eI;
	  break;
	}
      }
      if(phiPos == -1 && pfPhi_p->at(pI) == towPhiHi.at(towPhiHi.size()-1)) phiPos = towPhiHi.size()-1;

      etaPhiSum_[etaPos*nPhiTow + phiPos] += pfPt_p->at(pI);
      etaPhiSum_Collapse_[etaPosCollapse*nPhiTow + phiPos] += pfPt_p->at(pI);

      etaCent_[etaPos*nPhiTow + phiPos] = (towEtaLow.at(etaPos) + towEtaHi.at(etaPos))/2.;
      phiCent_[etaPos*nPhiTow + phiPos] = (towPhiLow.at(phiPos) + towPhiHi.at(phiPos))/2.;
    }


    if(!doJetStyle) learnTree_p->Fill();
    else{
      for(Int_t jI = 0; jI < nref_; ++jI){
	if(refpt_[jI] < 30.) continue;
	if(TMath::Abs(jteta_[jI]) > 2.) continue;

	jtptOut_ = jtpt_[jI];
	rawptOut_ = rawpt_[jI];
	jtphiOut_ = jtphi_[jI];
	jtetaOut_ = jteta_[jI];
	refptOut_ = refpt_[jI];

	for(Int_t i = 0; i < nJtPixels; ++i){jtPixels[i] = 0.0;}

	Int_t jtEtaPos = -1;
	Int_t jtPhiPos = -1;

	for(unsigned int eI = 0; eI < etaTowNPhi.size(); ++eI){
	  if(jtetaOut_ >= etaTowBounds.at(eI) && jtetaOut_ < etaTowBounds.at(eI+1)){
	    jtEtaPos = eI;
	    break;
	  }
	}

	for(Int_t eI = 0; eI < nPhiTow72; ++eI){
	  if(jtphiOut_ >= phiTow72[eI] && jtphiOut_ < phiTow72[eI+1]){
	    jtPhiPos = eI;
	    break;
	  }
	}

	for(Int_t pI = 0; pI < nJtPixels1D; ++pI){
	  Int_t currEtaPos = jtEtaPos - 5 + pI;

	  for(Int_t pI2 = 0; pI2 < nJtPixels1D; ++pI2){
	    Int_t currPhiPos = jtPhiPos - 5 + pI2;

	    for(unsigned int pfI = 0; pfI < pfPt_p->size(); ++pfI){
	      if(pfEta_p->at(pfI) >= etaTowBounds.at(currEtaPos) && pfEta_p->at(pfI) < etaTowBounds.at(currEtaPos+1)){
		if(pfPhi_p->at(pfI) >= phiTow72[currPhiPos] && pfPhi_p->at(pfI) < phiTow72[currPhiPos+1]){
		  jtPixels[pI*nJtPixels1D + pI2] += pfPt_p->at(pfI);
		}
	      }
	    }
	  }
	}


	learnTree_p->Fill();
      }
    }
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
  if(argc != 2 && argc != 3){
    std::cout << "Usage: ./treeFormat.exe <inFileName> <doJetStyle-optional>" << std::endl;
    return 1;
  }

  int retVal = 0;
  if(argc == 2) retVal += treeFormat(argv[1]);
  else if(argc == 3) retVal += treeFormat(argv[1], std::stoi(argv[2]));
  return retVal;
}
