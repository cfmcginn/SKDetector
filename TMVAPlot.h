//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Nov 20 19:25:28 2017 by ROOT version 6.08/07
// from TTree TestTree/TestTree
// found on file: /afs/cern.ch/work/s/skanaski/TMVAReg.root
//////////////////////////////////////////////////////////

#ifndef TMVAPlot_h
#define TMVAPlot_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class TMVAPlot {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           classID;
   Char_t          className[11];
   Float_t         jtpt;
   Float_t         jteta;
   Float_t         refpt;
   Float_t         weight;
   //   Float_t         BDT;
   //   Float_t         BDTG;
   Float_t         DNN;
   //   Float_t         LD;
   //   Float_t         MLP;

   // List of branches
   TBranch        *b_classID;   //!
   TBranch        *b_className;   //!
   TBranch        *b_jtpt;   //!
   TBranch        *b_jteta;
   TBranch        *b_refpt;   //!
   TBranch        *b_weight;   //!
   //   TBranch        *b_refpt_BDT;   //!
   //   TBranch        *b_refpt_BDTG;   //!
   TBranch        *b_refpt_DNN;   //!
   //   TBranch        *b_refpt_LD;   //!
   //   TBranch        *b_refpt_MLP;   //!

   TMVAPlot(TTree *tree=0);
   virtual ~TMVAPlot();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(std::string title_cent, std::string name_cent, std::string outpath);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef TMVAPlot_cxx
TMVAPlot::TMVAPlot(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/afs/cern.ch/work/s/skanaski/TMVAReg.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/afs/cern.ch/work/s/skanaski/TMVAReg.root");
      }
      TDirectory * dir = (TDirectory*)f->Get("/afs/cern.ch/work/s/skanaski/TMVAReg.root:/dataset");
      dir->GetObject("TestTree",tree);

   }
   Init(tree);
}

TMVAPlot::~TMVAPlot()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TMVAPlot::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TMVAPlot::LoadTree(Long64_t entry)
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

void TMVAPlot::Init(TTree *tree)
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

   fChain->SetBranchAddress("classID", &classID, &b_classID);
   fChain->SetBranchAddress("className", className, &b_className);
   fChain->SetBranchAddress("jtpt", &jtpt, &b_jtpt);
   fChain->SetBranchAddress("jteta", &jteta, &b_jteta);
   fChain->SetBranchAddress("refpt", &refpt, &b_refpt);
   fChain->SetBranchAddress("weight", &weight, &b_weight);
   //   fChain->SetBranchAddress("BDT", &BDT, &b_refpt_BDT);
   //   fChain->SetBranchAddress("BDTG", &BDTG, &b_refpt_BDTG);
   fChain->SetBranchAddress("DNN_CPU", &DNN, &b_refpt_DNN);
   //   fChain->SetBranchAddress("LD", &LD, &b_refpt_LD);
   //   fChain->SetBranchAddress("MLP", &MLP, &b_refpt_MLP);
   Notify();
}

Bool_t TMVAPlot::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TMVAPlot::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TMVAPlot::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef TMVAPlot_cxx
