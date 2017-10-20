//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sun Oct  1 19:48:36 2017 by ROOT version 6.08/06
// from TTree rhoTree/rhoTree
// found on file: HiForestAOD_1_20170929.root
//////////////////////////////////////////////////////////

#ifndef histplotter_h
#define histplotter_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class histplotter {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   UInt_t          run;
   UInt_t          lumi;
   UInt_t          evt;
   Int_t           hiBin;
   Double_t        skCutVal;
   Double_t        rhoVal;
   Int_t           nref;
   Float_t         jtpt[24];   //[nref]
   Float_t         jtphi[24];   //[nref]
   Float_t         jteta[24];   //[nref]
   Float_t         jtegfrac[24];   //[nref]
   Float_t         refpt[24];   //[nref]
   Float_t         refphi[24];   //[nref]
   Float_t         refeta[24];   //[nref]
   Float_t         refegfrac[24];   //[nref]
   Float_t         ref2pt[24];   //[nref]
   Float_t         ref2phi[24];   //[nref]
   Float_t         ref2eta[24];   //[nref]

   // List of branches
   TBranch        *b_run;   //!
   TBranch        *b_lumi;   //!
   TBranch        *b_evt;   //!
   TBranch        *b_hiBin;   //!
   TBranch        *b_skCutVal;   //!
   TBranch        *b_rhoVal;   //!
   TBranch        *b_nref;   //!
   TBranch        *b_jtpt;   //!
   TBranch        *b_jtphi;   //!
   TBranch        *b_jteta;   //!
   TBranch        *b_jtegfrac;   //!
   TBranch        *b_refpt;   //!
   TBranch        *b_refphi;   //!
   TBranch        *b_refeta;   //!
   TBranch        *b_refegfrac;   //!
   TBranch        *b_ref2pt;   //!
   TBranch        *b_ref2phi;   //!
   TBranch        *b_ref2eta;   //!

   histplotter(TTree *tree=0);
   virtual ~histplotter();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(std::string filepath="hists/");
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef histplotter_cxx
histplotter::histplotter(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("HiForestAOD_1_20170929.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("HiForestAOD_1_20170929.root");
      }
      f->GetObject("rhoTree",tree);

   }
   Init(tree);
}

histplotter::~histplotter()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t histplotter::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t histplotter::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void histplotter::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("run", &run, &b_run);
   fChain->SetBranchAddress("lumi", &lumi, &b_lumi);
   fChain->SetBranchAddress("evt", &evt, &b_evt);
   fChain->SetBranchAddress("hiBin", &hiBin, &b_hiBin);
   fChain->SetBranchAddress("skCutVal", &skCutVal, &b_skCutVal);
   fChain->SetBranchAddress("rhoVal", &rhoVal, &b_rhoVal);
   fChain->SetBranchAddress("nref", &nref, &b_nref);
   fChain->SetBranchAddress("jtpt", jtpt, &b_jtpt);
   fChain->SetBranchAddress("jtphi", jtphi, &b_jtphi);
   fChain->SetBranchAddress("jteta", jteta, &b_jteta);
   fChain->SetBranchAddress("jtegfrac", jtegfrac, &b_jtegfrac);
   fChain->SetBranchAddress("refpt", refpt, &b_refpt);
   fChain->SetBranchAddress("refphi", refphi, &b_refphi);
   fChain->SetBranchAddress("refeta", refeta, &b_refeta);
   fChain->SetBranchAddress("refegfrac", refegfrac, &b_refegfrac);
   fChain->SetBranchAddress("ref2pt", ref2pt, &b_ref2pt);
   fChain->SetBranchAddress("ref2phi", ref2phi, &b_ref2phi);
   fChain->SetBranchAddress("ref2eta", ref2eta, &b_ref2eta);
   Notify();
}

Bool_t histplotter::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void histplotter::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t histplotter::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef histplotter_cxx
