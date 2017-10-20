#define histplotter_cxx
#include "histplotter.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <map>
#include <fstream>

void histplotter::Loop(std::string filepath="hists/")
{
//   In a ROOT session, you can do:
//      root> .L histplotter.C
//      root> histplotter t
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

   std::ofstream fout("out.txt");

   // Root file for storing histograms
   std::string rootfilename=filepath+"out.root";
   TFile* outfile=new TFile(rootfilename.c_str(),"RECREATE");

   // Name and size info for vector of hists, et al.
   std::map<std::string,TH1F*> h_map;
   const int nCuts=3; // cent; eta; pt // If changed, don't forget to change cutData as appropriate at the beginning of the fill loop
   const int cutSize=7; // Maximum number of different situations for each cut
   std::string names[nCuts][cutSize]={ {"_centAll","_cent0","_cent30","_cent50","_cent70","EMPTY","EMPTY"},
				       {"_etaAll","_eta10","_eta5","_eta0","EMPTY","EMPTY","EMPTY"},
                                       {"_ptAll","_pt30","_pt35","_pt40","_pt50","_pt60", "_pt80"} };
   std::string titles[nCuts][cutSize]={ {", All Cent",", Cent 0-30",", Cent 30-50",", Cent 50-70",", Cent 70-100", "EMPTY", "EMPTY"},
					{", All Eta bins",", 1.0<abs(Eta)<1.6","0.5<abs(Eta)<1.0","abs(Eta)<0.5","EMPTY","EMPTY","EMPTY"},
                                        {", All pt bins",", pt 30-35",", pt 35-40",", pt 40-50",", pt 50-60",", pt 60-80",", pt 80-100"} };
   float cutValues[nCuts][cutSize]={ {0,60,100,140,200,-999,-999},
				     {-1.6,-1.0,-0.5,0,-999,-999,-999},
				     {30,35,40,50,60,80,100} };
   bool cuts[nCuts][cutSize];

   //   const int nBins=6;
   //   float binnings[nBins+1]={30,35,40,50,60,80,100};

   // Histograms
   std::vector<TH1F*> h_vec;
   std::vector<TF1*> f_vec;
   
   const int n_h2=2;
   std::vector<std::pair<int, int> > h2_specs={std::make_pair(2,0), std::make_pair(1,0)}; // bin var, graph var
   std::vector<vector<float> > h2_bins={ {30,35,40,50,60,90,100},
					 {0,0.5,1.0,1.6} }; // binnings for bin var
   std::string h2_titles[n_h2]={" by pt","by abs(eta)"};
   std::string h2_names[n_h2]={"_refpt_cent_","_absEta_cent_"};
   std::string h2_axisTitles[n_h2]={"refpt","abs(refeta)"};
   std::vector<std::vector<TH1F*> > h2_hists_Mean={ {}, {} };
   std::vector<std::vector<TH1F*> > h2_hists_StdDev={ {}, {} };

   std::vector<std::map<std::string,TH1F*> > h2_maps_Mean={ {}, {} };
   std::vector<std::map<std::string,TH1F*> > h2_maps_StdDev={ {}, {} };

   //   std::vector<TH1F*> h2_Mean_vec;
   //   std::vector<TH1F*> h2_StdDev_vec;
   // std::vector<TCanvas*> h2_Canvas_vec;

   Color_t colorList[14]={kRed, kGreen, kBlue, kYellow, kMagenta, kCyan, kOrange,
			  kSpring, kTeal, kAzure, kViolet, kPink, kGray, kBlack};

   // Loop to create histograms
   int maxCount=1;
   for (int i=0;i<nCuts;i++) {maxCount*=cutSize;}

   for (int i=0;i<maxCount;i++)
     {
       int indices[nCuts];
       int tmp=i;
       for (int j=1;j<=nCuts;j++)
         {
           indices[nCuts-j]=tmp%cutSize;
           tmp=(int) tmp/cutSize;
         }

       std::string name="";
       std::string title="Jet Energy Scale";
       std::string h2_name[n_h2];
       std::string h2_title[n_h2];
       bool exists=true;
       for (int j=0;j<nCuts;j++)
         {
           if (titles[j][indices[j]]=="EMPTY") exists=false;
           else
             {
               name += names[j][indices[j]];
               title += titles[j][indices[j]];
	       for (int k=0;k<n_h2;k++)
		 {
		   if (j!=h2_specs[k].first) h2_name[k] += names[j][indices[j]];
		   if (j==h2_specs[k].second) h2_title[k] += titles[j][indices[j]];
		 }
             }
         }

       if (exists)
         {
           //      std::cout<<"h_EScale"+name<<std::endl;
           TH1F* h_tmp=new TH1F(("h_jetEScale"+name).c_str(),title.c_str(),100,0,2);
           h_map["h_jetEScale"+name]=h_tmp;
           h_vec.push_back(h_tmp);
	   //           h_vec.push_back(h_tmp);
           f_vec.push_back(new TF1(("f_gaus1"+name).c_str(),"gaus",0.6,1.4));
	   //           f_vec.push_back(new TF1(("f_gaus2"+name).c_str(),"gaus",0.8,1.2));
	   for (int j=0;j<n_h2;j++)
	     {
	       if (indices[h2_specs[j].first]==0)
		 {
		   TH1F* h2_tmp=new TH1F(("h2"+h2_names[j]+"_jetEScaleMean"+name).c_str(),
					 ((h2_title[j].empty()?"":h2_title[j].substr(2))+";"+h2_axisTitles[j]).c_str(),
					 h2_bins[j].size()-1,&h2_bins[j][0]);
		   h2_maps_Mean[j][h2_name[j]]=h2_tmp;
		   //		   std::cout<<h2_name[j]<<std::endl;
		   h2_hists_Mean[j].push_back(h2_tmp);

		   h2_tmp=new TH1F(("h2"+h2_names[j]+"_jetEScaleStdDev"+name).c_str(),
				   ((h2_title[j].empty()?"":h2_title[j].substr(2))+";"+h2_axisTitles[j]).c_str(),
				   h2_bins[j].size()-1,&h2_bins[j][0]);
		   h2_maps_StdDev[j][h2_name[j]]=h2_tmp;
		   h2_hists_StdDev[j].push_back(h2_tmp);
		   
		   //		   h2_Mean_vec.push_back(new TH1F(("h2_refPt_jetEScaleMean"+name).c_str(),(h2_title.substr(2)+";refpt").c_str(),nBins,binnings));
		   //		   h2_StdDev_vec.push_back(new TH1F(("h2_refPt_jetEScaleStdDev"+name).c_str(),(h2_title.substr(2)+";refpt").c_str(),nBins,binnings));
		   //	       h2_Canvas_vec.push_back(new TCanvas());
		 }
	     }
         }
     }


   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      for (unsigned ii=0;ii<nref;ii++)
	{
	  double ptReco=jtpt[ii];
	  double ptGen=refpt[ii];

	  double cutData[nCuts]={(double)hiBin, -std::abs(refeta[ii]), ptGen};
	  
	  // Sets boolean cut values
	  for (unsigned i=0;i<nCuts;i++)
	    {
	      for (unsigned j=0;j<cutSize;j++)
		{
		  if (j==0) cuts[i][j]=cutData[i]>cutValues[i][0];
		  else
		    {
		      cuts[i][j]=(cutData[i]>cutValues[i][j-1])&&(cutData[i]<cutValues[i][j]);
		    }
		}
	    }

	  for (int i=0;i<maxCount;i++)
            {
              int indices[nCuts];
              int tmp=i;
              for (int j=1;j<=nCuts;j++)
                {
                  indices[nCuts-j]=tmp%cutSize;
                  tmp=(int) tmp/cutSize;
                }

              //              std::cout<<"*"<<std::endl;

              bool inCut=true;
              for (int j=0;j<nCuts;j++) {inCut = inCut && cuts[j][indices[j]];}

              if (inCut)
                {
                  std::string name="h_jetEScale";
                  for (int j=0;j<nCuts;j++) {name += names[j][indices[j]];}

		  //                  fout<<name<<", "<<ptGen<<", "<<hiBin/2<<std::endl;

                  h_map[name]->Fill(ptReco/ptGen);
                }
            }
        }
   }

   // Saves all histograms in root file and as pictures
   TCanvas* c1=new TCanvas("temp1","temp1",2800,8000);
   c1->Divide(7,20);

   /*   for (unsigned i=0;i<h_vec.size();i++)
     {
       c1->cd(i+1);
       auto h=h_vec.at(i);
       h->SetXTitle("Reco pt/Gen pt");
       //       h->SetMarkerStyle(kFullCircle);
       gStyle->SetOptFit(1);
       TLine* line2=new TLine(1,0,1,h->GetBinContent(h->GetMaximumBin()));
       line2->SetLineStyle(kDashed);
       h->Fit(f_vec.at(i)->GetName(),"R LL");
       h->Draw("e");
       line2->Draw();
     }*/
   unsigned h_index=0;

   // Fills h2 hists, draws 1d hists
   for (int i=0;i<maxCount;i++)
     {
       int indices[nCuts];
       int tmp=i;
       for (int j=1;j<=nCuts;j++)
         {
           indices[nCuts-j]=tmp%cutSize;
           tmp=(int) tmp/cutSize;
         }

       //    std::string name="";
       //    std::string title="Jet Energy Scale";
       std::string h2_name[n_h2];
       bool exists=true;
       for (int j=0;j<nCuts;j++)
         {
           if (titles[j][indices[j]]=="EMPTY") exists=false;
           else
             {
               //name += names[j][indices[j]];
               //title += titles[j][indices[j]];
	       for (int k=0;k<n_h2;k++) {if (j!=h2_specs[k].first) h2_name[k] += names[j][indices[j]];}
             }
         }

       if (exists)
	 {
	   c1->cd(h_index+1);
	   auto h=h_vec.at(h_index);
	   h->SetXTitle("Reco pt/Gen pt");
	   //       h->SetMarkerStyle(kFullCircle);
	   gStyle->SetOptFit(1);
	   TLine* line2=new TLine(1,0,1,h->GetBinContent(h->GetMaximumBin()));
	   line2->SetLineStyle(kDashed);
	   h->Fit(f_vec.at(h_index)->GetName(),"R LL");
	   h->Draw("e");
	   line2->Draw();
  
	   for (int j=0;j<n_h2;j++)
	     {
	       if (indices[h2_specs[j].first]!=0)
		 {
		   //		   std::cout<<h2_name[j]<<std::endl;
		   
		   auto h2_temp=h2_maps_Mean[j][h2_name[j]];
		   //		   if (h2_temp==0) std::cout<<h2_name[j]<<std::endl;
		   h2_temp->SetBinContent(indices[h2_specs[j].first],h->GetMean());
		   h2_temp->SetBinError(indices[h2_specs[j].first],h->GetMeanError());
		   
		   h2_temp=h2_maps_StdDev[j][h2_name[j]];
		   //		   if (h2_temp==0) std::cout<<h2_name[j]<<std::endl;
		   h2_temp->SetBinContent(indices[h2_specs[j].first],h->GetStdDev());
		   h2_temp->SetBinError(indices[h2_specs[j].first],h->GetStdDevError());
		   //	       fout<<h->GetName()<<", "<<cutValues[nCuts-1][indices[nCuts-1]-1]<<"-"<<cutValues[nCuts-1][indices[nCuts-1]]<<", "<<h->GetStdDev()<<"   "<<h2_temp->GetBinContent(indices[nCuts-1])<<std::endl;
		 }
	     }
	   	   
	   h_index++;
	 }
     }
   string filename="all_hists";
   filename=filepath+filename+".png";
   c1->SaveAs(filename.c_str());
   //   c1->Close();

   //   TCanvas* c_Mean[n_h2]={new TCanvas(), new TCanvas()};
   //   TCanvas* c_StdDev[n_h2]={new TCanvas(), new TCanvas()};
   gStyle->SetOptStat(0);
   
   //   TLegend* l_Mean[n_h2]={new TLegend(0.1,0.7,0.48,0.9), 
   //			  new TLegend(0.1,0.7,0.48,0.9)};
   //   TLegend* l_StdDev[n_h2]={new TLegend(0.1,0.7,0.48,0.9), 
   //			    new TLegend(0.1,0.7,0.48,0.9)};
   std::map<std::string, std::pair<TCanvas*, TLegend*> > map_Mean[n_h2];
   std::map<std::string, std::pair<TCanvas*, TLegend*> > map_StdDev[n_h2];

   for (int i=0;i<maxCount;i++)
     {
       int indices[nCuts];
       int tmp=i;
       for (int j=1;j<=nCuts;j++)
         {
           indices[nCuts-j]=tmp%cutSize;
           tmp=(int) tmp/cutSize;
         }

       //    std::string name="";
       //    std::string title="Jet Energy Scale";
       std::string h2_name[n_h2];
       std::string h2_fileName[n_h2];
       std::string h2_title[n_h2];
       bool exists=true;
       for (int j=0;j<nCuts;j++)
         {
           if (titles[j][indices[j]]=="EMPTY") exists=false;
           else
             {
               //name += names[j][indices[j]];
               //title += titles[j][indices[j]];
               for (int k=0;k<n_h2;k++) 
		 {
		   if (j!=h2_specs[k].first) 
		     {
		       h2_name[k] += names[j][indices[j]];
		       if (j!=h2_specs[k].second) 
			 {
			   h2_fileName[k] += names[j][indices[j]];
			   h2_title[k] += titles[j][indices[j]];
			 }
		     }
		 }
             }
         }

       if (exists)
         {
	   for (int j=0;j<n_h2;j++)
	     {
	       if (indices[h2_specs[j].first]==0)
		 {
		   if (map_Mean[j].count(h2_fileName[j])==0)
		     {
		       map_Mean[j][h2_fileName[j]] = std::make_pair(new TCanvas(("h2_Mean"+h2_names[j]+h2_fileName[j]).c_str(),"",800,800), new TLegend(0.1, 0.7, 0.48, 0.9));
		     }
		   TCanvas* c_Mean=map_Mean[j][h2_fileName[j]].first;
		   TLegend* l_Mean=map_Mean[j][h2_fileName[j]].second;
		   c_Mean->cd();
		   auto h2_temp=h2_maps_Mean[j][h2_name[j]];
		   l_Mean->AddEntry(h2_temp, h2_temp->GetTitle(), "p");
		   h2_temp->SetTitle(("Mean Jet Energy Scale"+h2_title[j]).c_str());
		   h2_temp->SetMinimum(0.8);
		   h2_temp->SetMaximum(1.2);
		   h2_temp->SetMarkerColor(colorList[indices[h2_specs[j].second]]);
		   h2_temp->SetMarkerStyle(kFullCircle);
		   h2_temp->Draw("e same");
		   
		   if (map_StdDev[j].count(h2_fileName[j])==0)
		     {
		       map_StdDev[j][h2_fileName[j]] = std::make_pair(new TCanvas(("h2_StdDev"+h2_names[j]+h2_fileName[j]).c_str(),"",400,400), new TLegend(0.1, 0.7, 0.48, 0.9));
		     }
		   TCanvas* c_StdDev=map_StdDev[j][h2_fileName[j]].first;
		   TLegend* l_StdDev=map_StdDev[j][h2_fileName[j]].second;
		   c_StdDev->cd();
		   h2_temp=h2_maps_StdDev[j][h2_name[j]];
		   l_StdDev->AddEntry(h2_temp, h2_temp->GetTitle(), "p");
		   h2_temp->SetTitle(("Std Dev of Jet Energy Scale"+h2_title[j]).c_str());
		   h2_temp->SetMinimum(0);
		   h2_temp->SetMaximum(0.5);
		   h2_temp->SetMarkerColor(colorList[indices[h2_specs[j].second]]);
		   h2_temp->SetMarkerStyle(kFullCircle);
		   h2_temp->Draw("e same");
		 }
	     }
	 }
     }
   for (int i=0;i<n_h2;i++)
     {
       for (auto cl_pair : map_Mean[i])
	 {
	   TCanvas* c=cl_pair.second.first;
	   TLegend* l=cl_pair.second.second;
	   
	   c->cd();
	   l->Draw();
	   
	   string  filename=filepath+c->GetName()+".png";
	   c->SaveAs(filename.c_str());
	 }
       for (auto cl_pair : map_StdDev[i])
	 {
	   TCanvas* c=cl_pair.second.first;
	   TLegend* l=cl_pair.second.second;
	   
	   c->cd();
	   l->Draw();
	   
	   string filename=filepath+c->GetName()+".png";
	   c->SaveAs(filename.c_str());
	 }
     }

   /*for (int i=0;i<h2_Mean_vec.size();i++)
     {
       c_Mean->cd();
       auto h2_temp=h2_Mean_vec.at(i);
       l_Mean->AddEntry(h2_temp, h2_temp->GetTitle(), "p");
       h2_temp->SetTitle("Mean Jet Energy Scale");
       h2_temp->SetMinimum(0.8);
       h2_temp->SetMaximum(1.2);
       h2_temp->SetMarkerColor(colorList[i]);
       h2_temp->SetMarkerStyle(kFullCircle);
       h2_temp->Draw("e same");

       c_StdDev->cd();
       h2_temp=h2_StdDev_vec.at(i);
       l_StdDev->AddEntry(h2_temp, h2_temp->GetTitle(), "p");
       h2_temp->SetTitle("Std Dev of Jet Energy Scale");
       h2_temp->SetMinimum(0);
       h2_temp->SetMaximum(0.5);
       h2_temp->SetMarkerColor(colorList[i]);
       h2_temp->SetMarkerStyle(kFullCircle);
       h2_temp->Draw("e same");
     }
   c_Mean->cd();
   l_Mean->Draw();

   c_StdDev->cd();
   l_StdDev->Draw();

   filename="h2_Mean";
   filename=filepath+filename+".png";
   c_Mean->SaveAs(filename.c_str());
   //   c_Mean->Close();

   filename="h2_StdDev";
   filename=filepath+filename+".png";
   c_StdDev->SaveAs(filename.c_str());
   //   c_StdDev->Close();
   */   

   outfile->Write();
   outfile->Close();
}
