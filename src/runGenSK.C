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
#include "TH1F.h"
#include "TH2F.h"
#include "TRandom3.h"
#include "TMath.h"
#include "TLorentzVector.h"

//Local dependencies
#include "include/doGlobalDebug.h"
#include "include/genParticleClass.h"
#include "include/jetClass.h"
#include "include/getLinBins.h"
#include "include/etaPhiFunc.h"
#include "include/pdgToMassInGeV.h"
#include "include/checkMakeDir.h"
#include "include/returnFileList.h"
#include "include/returnFileListFromTxt.h"

#include "fastjet/ClusterSequence.hh"

void prettyTH2(TH2* inHist_p)
{
  inHist_p->GetXaxis()->CenterTitle();
  inHist_p->GetYaxis()->CenterTitle();

  return;
}

void sortVects(std::vector<double>* sortV_p, std::vector<double>* v1_p, std::vector<double>* v2_p)
{
  for(unsigned int i = 0; i < sortV_p->size(); ++i){
    for(unsigned int j = i+1; j < sortV_p->size(); ++j){

      if(sortV_p->at(i) < sortV_p->at(j)){
	double tempV = sortV_p->at(i);
	double tempV1 = v1_p->at(i);
	double tempV2 = v2_p->at(i);

	sortV_p->at(i) = sortV_p->at(j);
	v1_p->at(i) = v1_p->at(j);
	v2_p->at(i) = v2_p->at(j);

	sortV_p->at(j) = tempV;
	v1_p->at(j) = tempV1;
	v2_p->at(j) = tempV2;
      }
    }
  }

  return;
}


int runGenSK(const std::string inFileName, std::string outFileName = "")
{
  //For iterating over files or directories, depending on input string
  std::vector<std::string> fileList;
  if(isFileTxt(inFileName)) fileList = returnFileListFromTxt(inFileName, "HiForestAOD_");
  else if(checkFile(inFileName)) fileList.push_back(inFileName);
  else if(checkDir(inFileName)) fileList = returnFileList(inFileName, "HiForestAOD_");
  else{
    std::cout << "inFileName \'" << inFileName << "\' is not a valid root file, txt file, or directory. return 1" << std::endl;
    return 1;
  }

  if(fileList.size() == 0){
    std::cout << "inFileName \'" << inFileName << "\' is gave no valid files. return 1" << std::endl;
    return 1;
  }
  std::cout << "FileList size: " << fileList.size() << std::endl;

  //do a little string handling for convenient filenames
  TDatime* date = new TDatime();

  if(doGlobalDebug) std::cout << __FILE__ << ", " << __LINE__ << std::endl;

  if(outFileName.size() == 0) outFileName = inFileName;
  while(outFileName.find("/") != std::string::npos){outFileName.replace(0,outFileName.find("/")+1,"");}
  if(outFileName.find(".root") != std::string::npos) outFileName.replace(outFileName.find(".root"),5,"");
  if(outFileName.find(".txt") != std::string::npos) outFileName.replace(outFileName.find(".txt"),4,"");
  outFileName = outFileName + "_" + std::to_string(date->GetDate()) + ".root";
  delete date;

  //output file + other necessities
  TFile* outFile_p = new TFile(outFileName.c_str(), "RECREATE");

  pdgToMassInGeV massMap;

  UInt_t run_;
  UInt_t lumi_;
  ULong64_t evt_;

  Int_t hiBin;
  Double_t skCutVal;
  Double_t rhoVal;

  jetClass jets;

  TTree* outTree_p = new TTree("rhoTree", "rhoTree");
  outTree_p->Branch("run", &run_, "run/i");
  outTree_p->Branch("lumi", &lumi_, "lumi/i");
  outTree_p->Branch("evt", &evt_, "evt/i");
  outTree_p->Branch("hiBin", &hiBin, "hiBin/I");
  outTree_p->Branch("skCutVal", &skCutVal, "skCutVal/D");
  outTree_p->Branch("rhoVal", &rhoVal, "rhoVal/D");

  outTree_p->Branch("nref", &jets.nref_, "nref/I");
  outTree_p->Branch("jtpt", jets.jtpt_, "jtpt[nref]/F");
  outTree_p->Branch("jtphi", jets.jtphi_, "jtphi[nref]/F");
  outTree_p->Branch("jteta", jets.jteta_, "jteta[nref]/F");
  outTree_p->Branch("jtegfrac", jets.jtegfrac_, "jtegfrac[nref]/F");
  outTree_p->Branch("refpt", jets.refpt_, "refpt[nref]/F");
  outTree_p->Branch("refphi", jets.refphi_, "refphi[nref]/F");
  outTree_p->Branch("refeta", jets.refeta_, "refeta[nref]/F");
  outTree_p->Branch("refegfrac", jets.refegfrac_, "refegfrac[nref]/F");
  outTree_p->Branch("ref2pt", jets.ref2pt_, "ref2pt[nref]/F");
  outTree_p->Branch("ref2phi", jets.ref2phi_, "ref2phi[nref]/F");
  outTree_p->Branch("ref2eta", jets.ref2eta_, "ref2eta[nref]/F");

  //Fastjet defs
  const double jetR = 0.4;
  fastjet::JetDefinition jetDef(fastjet::antikt_algorithm, jetR);
  std::vector<fastjet::PseudoJet> constituents;

  //MST periodicity generator for random cones
  TRandom3* randGen_p = new TRandom3(0);
  const Int_t nRC = 11;
  const Double_t absEtaMaxRC = 2.;
  const Double_t rcConeSize = .4;

  TH1F* rho_h = new TH1F("rho_h", ";#rho;Events", 300, 0, 300);
  TH2F* rhoVsSKCut_p = new TH2F("rhoVsSKCut_h", ";#rho;SK Cut (GeV)", 100, 0, 300, 100, 0, 5);
  prettyTH2(rhoVsSKCut_p);

  if(doGlobalDebug) std::cout << __FILE__ << ", " << __LINE__ << std::endl;

  for(unsigned int fileIter = 0; fileIter < fileList.size(); ++fileIter){
    TFile* inFile_p = new TFile(fileList.at(fileIter).c_str(), "READ");
    TTree* genTree_p = (TTree*)inFile_p->Get("HiGenParticleAna/hi");
    TTree* hiTree_p = (TTree*)inFile_p->Get("hiEvtAnalyzer/HiTree");
    TTree* jetTree_p = (TTree*)inFile_p->Get("akPu4PFJetAnalyzer/t");
    
    genParticleClass particles;
    
    const Int_t nMaxJets = 500;
    Int_t ngen_;
    Float_t genpt_[nMaxJets];
    Float_t genphi_[nMaxJets];
    Float_t geneta_[nMaxJets];
    Int_t gensubid_[nMaxJets];
    
    genTree_p->SetBranchStatus("*", 0);
    genTree_p->SetBranchStatus("pt", 1);
    genTree_p->SetBranchStatus("phi", 1);
    genTree_p->SetBranchStatus("eta", 1);
    genTree_p->SetBranchStatus("chg", 1);
    genTree_p->SetBranchStatus("pdg", 1);
    genTree_p->SetBranchStatus("sube", 1);
    
    genTree_p->SetBranchAddress("pt", &(particles.pt_p));
    genTree_p->SetBranchAddress("phi", &(particles.phi_p));
    genTree_p->SetBranchAddress("eta", &(particles.eta_p));
    genTree_p->SetBranchAddress("chg", &(particles.chg_p));
    genTree_p->SetBranchAddress("pdg", &(particles.pdg_p));
    genTree_p->SetBranchAddress("sube", &(particles.sube_p));
    
    hiTree_p->SetBranchStatus("*", 0);
    hiTree_p->SetBranchStatus("run", 1);
    hiTree_p->SetBranchStatus("lumi", 1);
    hiTree_p->SetBranchStatus("evt", 1);
    hiTree_p->SetBranchStatus("hiBin", 1);
    
    hiTree_p->SetBranchAddress("run", &run_);
    hiTree_p->SetBranchAddress("lumi", &lumi_);
    hiTree_p->SetBranchAddress("evt", &evt_);
    hiTree_p->SetBranchAddress("hiBin", &hiBin);
    
    jetTree_p->SetBranchStatus("*", 0);
    jetTree_p->SetBranchStatus("ngen", 1);
    jetTree_p->SetBranchStatus("genpt", 1);
    jetTree_p->SetBranchStatus("genphi", 1);
    jetTree_p->SetBranchStatus("geneta", 1);
    jetTree_p->SetBranchStatus("gensubid", 1);
    
    jetTree_p->SetBranchAddress("ngen", &ngen_);
    jetTree_p->SetBranchAddress("genpt", genpt_);
    jetTree_p->SetBranchAddress("genphi", genphi_);
    jetTree_p->SetBranchAddress("geneta", geneta_);
    jetTree_p->SetBranchAddress("gensubid", gensubid_);
    
    const Int_t nEntries = TMath::Min(1000000, (Int_t)genTree_p->GetEntries());

    //Construct SK grid
    const Int_t nSKEtaBins = 12;
    const Int_t nSKPhiBins = 16;
    Double_t skEtaBins[nSKEtaBins+1];
    Double_t skPhiBins[nSKPhiBins+1];
    getLinBins(-absEtaMaxRC-.4, absEtaMaxRC+.4, nSKEtaBins, skEtaBins);
    getLinBins(-TMath::Pi(), TMath::Pi(), nSKPhiBins, skPhiBins);
    
    std::cout << "Eta bins: " << std::endl;
    for(int i = 0; i < nSKEtaBins+1; ++i){std::cout << " " << skEtaBins[i] << ",";}
    std::cout << std::endl;
    
    std::cout << "Phi bins: " << std::endl;
    for(int i = 0; i < nSKPhiBins+1; ++i){std::cout << " " << skPhiBins[i] << ",";}
    std::cout << std::endl;
    
    Double_t maxTrackVal[nSKEtaBins][nSKPhiBins];
    
    std::cout << "Processing " << nEntries << " events..." << std::endl;
    
    if(doGlobalDebug) std::cout << __FILE__ << ", " << __LINE__ << std::endl;

    clock_t t;
    t = clock();
    const double clockPrintInterval = 10;
    
    for(Int_t entry = 0; entry < nEntries; ++entry){
      if((clock() - t)/CLOCKS_PER_SEC > clockPrintInterval){
	std::cout << " Entry: " << entry << "/" << nEntries << std::endl;
	t = clock();
      }
      genTree_p->GetEntry(entry);
      hiTree_p->GetEntry(entry);
      jetTree_p->GetEntry(entry);
      
    //clear prev. grid
      for(int i = 0; i < nSKEtaBins; ++i){
	for(int j = 0; j < nSKPhiBins; ++j){maxTrackVal[i][j] = 0;}
      }
      
      for(int gI = 0; gI < (int)(particles.pt_p->size()); ++gI){
	if(TMath::Abs(particles.pdg_p->at(gI)) == 12) continue;
	if(TMath::Abs(particles.pdg_p->at(gI)) == 14) continue;
	if(TMath::Abs(particles.pdg_p->at(gI)) == 16) continue;
	
	if(TMath::Abs(particles.eta_p->at(gI)) >= absEtaMaxRC+.4) continue;
	
	if(particles.sube_p->at(gI) == 0){
	  TLorentzVector temp;
	  temp.SetPtEtaPhiM(particles.pt_p->at(gI), particles.eta_p->at(gI), particles.phi_p->at(gI), massMap.getMassFromID(particles.pdg_p->at(gI)));
	  fastjet::PseudoJet tempPJ(temp.Px(), temp.Py(), temp.Pz(), temp.E());
	  tempPJ.set_user_index(particles.pdg_p->at(gI));
	  constituents.push_back(tempPJ);
	}
	
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
	if(phiPos < 0 && particles.phi_p->at(gI) >= skPhiBins[nSKPhiBins]) phiPos = nSKPhiBins-1;
	
	if(etaPos == -1) std::cout << "ERROR! etaPos < 0" << std::endl;
	if(phiPos == -1) std::cout << "ERROR! phiPos < 0" << std::endl;
	
	if(particles.pt_p->at(gI) > maxTrackVal[etaPos][phiPos]) maxTrackVal[etaPos][phiPos] = particles.pt_p->at(gI);
      }
      
      fastjet::ClusterSequence cs(constituents, jetDef);
      std::vector<fastjet::PseudoJet> genJets = fastjet::sorted_by_pt(cs.inclusive_jets());
      constituents.clear();
      
      std::vector<double> skVect;
      //    std::cout << "CHECK GRID A: " << std::endl;
      for(int i = 0; i < nSKEtaBins; ++i){
	for(int j = 0; j < nSKPhiBins; ++j){skVect.push_back(maxTrackVal[i][j]);// std::cout << " (" << skPhiBins[j] << "-" << skPhiBins[j+1] << ")" << maxTrackVal[i][j] << ", ";}
	}
      }
      //    std::cout << std::endl;
      //    std::cout << std::endl;
      
      std::sort(std::begin(skVect), std::end(skVect));
      skCutVal = (skVect.at(skVect.size()/2 - 1) + skVect.at(skVect.size()/2))/2.;
      //    if(skCutVal < .5) skCutVal = .75;
      
      //    std::cout << "CHECK GRID" << std::endl;
      for(int i = 0; i < (int)skVect.size(); ++i){
	//      std::cout << " " << skVect.at(i) << ", ";
      }
      //    std::cout << std::endl;
      //    std::cout << std::endl;
      
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
	if(TMath::Abs(particles.pdg_p->at(gI)) == 12) continue;
	if(TMath::Abs(particles.pdg_p->at(gI)) == 14) continue;
	if(TMath::Abs(particles.pdg_p->at(gI)) == 16) continue;
	
	for(unsigned int i = 0; i < etaRCVals.size(); ++i){
	  if(getDR(etaRCVals.at(i), phiRCVals.at(i), particles.eta_p->at(gI), particles.phi_p->at(gI)) < rcConeSize){
	    sumRCVals.at(i) += particles.pt_p->at(gI);
	  }
	}
	
	if(particles.pt_p->at(gI) > skCutVal){
	  ptPastSK_.push_back(particles.pt_p->at(gI));
	  phiPastSK_.push_back(particles.phi_p->at(gI));
	  etaPastSK_.push_back(particles.eta_p->at(gI));
	  
	  TLorentzVector temp;
	  temp.SetPtEtaPhiM(particles.pt_p->at(gI), particles.eta_p->at(gI), particles.phi_p->at(gI), massMap.getMassFromID(particles.pdg_p->at(gI)));
	  fastjet::PseudoJet tempPJ(temp.Px(), temp.Py(), temp.Pz(), temp.E());
	  tempPJ.set_user_index(particles.pdg_p->at(gI));
	  constituents.push_back(tempPJ);
	}
      }
      
      fastjet::ClusterSequence cs2(constituents, jetDef);
      std::vector<fastjet::PseudoJet> subJets = fastjet::sorted_by_pt(cs2.inclusive_jets());
      constituents.clear();
      
      for(int gI = 0; gI < (int)ptPastSK_.size(); ++gI){
	for(unsigned int i = 0; i < etaRCVals.size(); ++i){
	  if(getDR(etaRCVals.at(i), phiRCVals.at(i), etaPastSK_.at(gI), phiPastSK_.at(gI)) < rcConeSize){
	    sumRCSKVals.at(i) += ptPastSK_.at(gI);
	  }
	}
      } 
      
      
      sortVects(&sumRCVals, &etaRCVals, &phiRCVals);    
      rhoVal = (sumRCVals.at(sumRCVals.size()/2))/(TMath::Pi()*rcConeSize*rcConeSize);
      rho_h->Fill(rhoVal);
      rhoVsSKCut_p->Fill(rhoVal, skCutVal);
      
      bool isUsed[ngen_];
      for(int j = 0; j < ngen_; ++j){
	isUsed[j] = false;
      }
      
      jets.nref_ = 0;
      for(unsigned int i = 0; i < subJets.size(); ++i){
	if(subJets.at(i).pt() < 10.) break;
	
	jets.jtpt_[jets.nref_] = subJets.at(i).pt();
	jets.jtphi_[jets.nref_] = subJets.at(i).phi_std();
	jets.jteta_[jets.nref_] = subJets.at(i).eta();
	
	std::vector<fastjet::PseudoJet> jetConst = subJets.at(i).constituents();
	jets.jtegfrac_[jets.nref_] = 0;
	
	for(unsigned int j = 0; j < jetConst.size(); ++j){
	  if(jetConst.at(j).user_index() == 22 && jetConst.at(j).pt()/jets.jtpt_[jets.nref_] > jets.jtegfrac_[jets.nref_]) jets.jtegfrac_[jets.nref_] = jetConst.at(j).pt()/jets.jtpt_[jets.nref_];
	}

	if(doGlobalDebug) std::cout << __FILE__ << ", " << __LINE__ << std::endl;
	
	jets.refpt_[jets.nref_] = -999;
	jets.refphi_[jets.nref_] = -999;
	jets.refeta_[jets.nref_] = -999;
	jets.refegfrac_[jets.nref_] = -999;
	
	jets.ref2pt_[jets.nref_] = -999;
	jets.ref2phi_[jets.nref_] = -999;
	jets.ref2eta_[jets.nref_] = -999;
	
	for(unsigned int j = 0; j < genJets.size(); ++j){
	  if(getDR(subJets.at(i).eta(), subJets.at(i).phi_std(), genJets.at(j).eta(), genJets.at(j).phi_std()) < jetR){
	    jets.refpt_[jets.nref_] = genJets.at(j).pt();
	    jets.refphi_[jets.nref_] = genJets.at(j).phi_std();
	    jets.refeta_[jets.nref_] = genJets.at(j).eta();
	    
	    std::vector<fastjet::PseudoJet> refConst = genJets.at(j).constituents();
	    jets.refegfrac_[jets.nref_] = 0;
	    
	    for(unsigned int k = 0; k < refConst.size(); ++k){
	      if(refConst.at(k).user_index() == 22 && refConst.at(k).pt()/jets.refpt_[jets.nref_] > jets.refegfrac_[jets.nref_]) jets.refegfrac_[jets.nref_] = refConst.at(k).pt()/jets.refpt_[jets.nref_];
	    }
	    
	    genJets.erase(genJets.begin()+j);
	    break;
	  }
	}
	
	for(int j = 0; j < ngen_; ++j){
	  if(gensubid_[j] != 0) continue;
	  if(isUsed[j]) continue;
	  if(getDR(subJets.at(i).eta(), subJets.at(i).phi_std(), geneta_[j], genphi_[j]) < jetR){
	    jets.ref2pt_[jets.nref_] = genpt_[j];
	    jets.ref2phi_[jets.nref_] = genphi_[j];
	    jets.ref2eta_[jets.nref_] = geneta_[j];
	    
	    isUsed[j] = true;
	    
	    break;
	  }
	}
	
	++jets.nref_;
      }
      
      outTree_p->Fill();
    }

    if(doGlobalDebug) std::cout << __FILE__ << ", " << __LINE__ << std::endl;
    
    inFile_p->Close();
    delete inFile_p;
  }
  std::cout << "Processing complete." << std::endl;

  if(doGlobalDebug) std::cout << __FILE__ << ", " << __LINE__ << std::endl;

  outFile_p->cd();

  outTree_p->Write("", TObject::kOverwrite);
  delete outTree_p;

  rho_h->Write("", TObject::kOverwrite);
  delete rho_h;

  rhoVsSKCut_p->Write("", TObject::kOverwrite);
  delete rhoVsSKCut_p;

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
