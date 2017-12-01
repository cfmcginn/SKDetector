#define TMVAPlot_cxx
#include "TMVAPlot.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <map>

void TMVAPlot::Loop(std::string title_cent, std::string name_cent)
{
//   In a ROOT session, you can do:
//      root> .L TMVAPlot.C
//      root> TMVAPlot t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   TFile* outfile=new TFile("hists_tmva/out_cent0.root","RECREATE");
   
   // histograms
   //   std::string title_cent=", Cent 0-30";
   //   std::string name_cent="_cent0";
   //   std::string title_pt=", pt 30-50";
   //   std::string name_pt="_pt30";
   const int nEtaCuts=3;
   float etaCuts[]={0,0.5,1.0,2.0};
   std::string titles_eta[]={", abs(eta)<0.5",", 0.5<abs(eta)<1.0",", 1.0<abs(eta)<2.0"};
   std::string names_eta[]={"_eta0","_eta5","_eta10"};
   
   const int nPtCuts=3;
   float ptCuts[]={30,50,70,100};
   std::string titles_pt[]={", pt 30-50",", pt 50-70", ", pt 70-100"};
   std::string names_pt[]={"_pt30","_pt50","_pt70"};
   
   for (int j=0;j<nPtCuts;j++){
     TH1F* h_mEScale_all=new TH1F(("h_mEScale"+name_cent+names_pt[j]).c_str(), ("Jet EScale"+title_cent+titles_pt[j]+", all eta;reco/gen").c_str(),100,0,2);
     TH1F* h_EScale_all=new TH1F(("h_EScale"+name_cent+names_pt[j]).c_str(), ("Jet EScale"+title_cent+titles_pt[j]+", all eta;reco/gen").c_str(),100,0,2);
     TH1F* h_mCorr_all=new TH1F(("h_mCorr"+name_cent+names_pt[j]).c_str(), ("Energy Correction"+title_cent+titles_pt[j]+", all eta;DNN/jtpt").c_str(),100,0,2);
     
     std::map<std::string, TH1F*> hist_map_mEScale;
     std::map<std::string, TH1F*> hist_map_EScale;
     std::map<std::string, TH1F*> hist_map_mCorr;
     for (int i=0;i<nEtaCuts;i++)
       {
	 hist_map_mEScale[names_eta[i]]=new TH1F(("h_mEScale"+name_cent+names_pt[j]+names_eta[i]).c_str(), ("Jet EScale"+title_cent+titles_pt[j]+titles_eta[i]+";reco/gen").c_str(),100,0,2);
	 hist_map_EScale[names_eta[i]]=new TH1F(("h_EScale"+name_cent+names_pt[j]+names_eta[i]).c_str(), ("Jet EScale"+title_cent+titles_pt[j]+titles_eta[i]+";reco/gen").c_str(),100,0,2);
	 hist_map_mCorr[names_eta[i]]=new TH1F(("h_mCorr"+name_cent+names_pt[j]+names_eta[i]).c_str(), ("Energy Correction"+title_cent+titles_pt[j]+titles_eta[i]+";DNN/jtpt").c_str(),100,0,2);
       }

     Long64_t nentries = fChain->GetEntriesFast();
     
     Long64_t nbytes = 0, nb = 0;
     for (Long64_t jentry=0; jentry<nentries;jentry++) {
       Long64_t ientry = LoadTree(jentry);
       if (ientry < 0) break;
       nb = fChain->GetEntry(jentry);   nbytes += nb;
       // if (Cut(ientry) < 0) continue;
       
       if (refpt>ptCuts[j]&&refpt<ptCuts[j+1])
	 {
	   h_mEScale_all->Fill(DNN/refpt);
	   h_EScale_all->Fill(jtpt/refpt);
	   h_mCorr_all->Fill(DNN/jtpt);
	   
	   for (int i=0;i<nEtaCuts;i++)
	     {
	       if (std::abs(jteta)>etaCuts[i]&&std::abs(jteta)<etaCuts[i+1])
		 {
		   hist_map_mEScale[names_eta[i]]->Fill(DNN/refpt);
		   hist_map_EScale[names_eta[i]]->Fill(jtpt/refpt);
		   hist_map_mCorr[names_eta[i]]->Fill(DNN/jtpt);
		 }
	     }
	 }
     }
     
     TCanvas* c1=new TCanvas("c1", "c1", 800, 800);
     
     h_mEScale_all->Draw();
     h_EScale_all->Draw("*hist same");
     h_mEScale_all->Draw("same");
     c1->SaveAs(("hists_tmva/"+(std::string)h_EScale_all->GetName()+".png").c_str());
     
     h_mCorr_all->Draw();
     c1->SaveAs(("hists_tmva/"+(std::string)h_mCorr_all->GetName()+".png").c_str());
     
     for (int i=0;i<nEtaCuts;i++)
       {
	 TH1F* h_mEScale=hist_map_mEScale[names_eta[i]];
	 TH1F* h_EScale=hist_map_EScale[names_eta[i]];
	 TH1F* h_mCorr=hist_map_mCorr[names_eta[i]];
	 
	 h_mEScale->Draw();
	 h_EScale->Draw("*hist same");
	 h_mEScale->Draw("same");
	 c1->SaveAs(("hists_tmva/"+(std::string)h_EScale->GetName()+".png").c_str());
	 
	 h_mCorr->Draw();
	 c1->SaveAs(("hists_tmva/"+(std::string)h_mCorr->GetName()+".png").c_str());
       }
     /*   for (std::string name : plotList)
	  {
	  hist_map[name]->Draw();
	  c1->SaveAs(("hists_tmva/"+(std::string)hist_map[name]->GetName()+"_cent70.png").c_str());
	  }*/
   }
   outfile->Write();
   outfile->Close();
}
