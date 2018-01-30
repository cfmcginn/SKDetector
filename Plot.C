#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>

#include "TFile.h"
#include "TTree.h"
#include "TDatime.h"
#include "TObjArray.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TPad.h"

#include "include/returnRootFileContentsList.h"
#include "kirchnerPalette.h"
#include "CustomCanvas.h"
#include "TMVASlides.C"

void Plot(std::string location, bool hi=true, std::string slideName="PlotSlides.tex", std::string treeName="t")
{
  int nCentCuts;
  std::vector<float> centCuts;
  std::vector<std::string> centNames;
  std::vector<std::string> centTitles;

  if (hi)
    {
      nCentCuts=5;
      centCuts={0, 20, 60, 100, 140, 200};
      centNames={"_cent0", "_cent10", "_cent30", "_cent50", "_cent70"};
      centTitles={", Cent 0-10", ", Cent 10-30", ", Cent 30-50", ", Cent 50-70", ", Cent 70-100"};
    }
  else
    {
      nCentCuts=1;
      centCuts={-999,0};
      centNames={"_all"};
      centTitles={", All Cent"};
    }

  std::map<std::string, std::vector<TH1F*>* > m_histVectors;
  std::map<std::string, TFile*> m_outFiles;
  std::map<std::string, long[5]> m_evtCounts;

  kirchnerPalette* kP=new kirchnerPalette();
  if (nCentCuts>6)
    {
      std::cout<<"WARNING: This many centrality bins is not supported by the Kirchner Palette.  Please revise the color scheme."<<std::endl;
      return;
    }

  std::vector<Style_t> styleList={kOpenCircle, kOpenTriangleUp, kOpenSquare, kOpenDiamond, kOpenCross, kPlus, kMultiply, kOpenStar};
  if (nCentCuts>styleList.size())
    {
      std::cout<<"WARNING: Number of centrality bins outnumbers number of preset marker styles.  Please revise."<<std::endl;
      return;
    }

  //  if (location.back()=="/") location=location.substr(0,location.length()-1);

  gSystem->Exec(("ls "+location+" > tmp.txt").c_str());

  std::fstream fileList("tmp.txt");
  int nFiles=0;
  while (!fileList.eof())
    {
      std::string inputFileName;
      fileList>>inputFileName;
      
      if (inputFileName.length()==0) continue;

      std::cout<<"Processing file "<<inputFileName<<std::endl;

      if (inputFileName.find("MERGED")!=std::string::npos)
	{
	  std::cout<<"File "<<inputFileName<<" ignored"<<std::endl;
	  continue;
	}
      
      TFile* inputFile=new TFile((location+"/"+inputFileName).c_str());
      if (inputFile==0)
	{
	  std::cout<<"ERROR: File "<<inputFileName<<" not found"<<std::endl;
	  continue;
	}

      TTree* HiTree=(TTree*)inputFile->Get("hiEvtAnalyzer/HiTree");
      if (HiTree==0)
	{
	  std::cout<<"ERROR: Could not find HiTree and/or hiEvtAnalyzer"<<std::endl;
	  std::cout<<"WARNING: Exiting file "<<inputFileName<<" because HiTree could not be found"<<std::endl;
	  inputFile->Close();
	  continue;
	}

      Int_t hiBin;
      TBranch* b_hiBin;
      HiTree->SetBranchAddress("hiBin", &hiBin, &b_hiBin);
      
      std::vector<std::string> dirList=returnRootFileContentsList(inputFile, "TDirectoryFile", "JetAnalyzer");
      for (int iDir=0;iDir<dirList.size();iDir++)
	{
	  std::string dirName=dirList.at(iDir);

	  std::cout<<"  TDirectoryFile "<<dirName<<std::endl;

	  if (nFiles==0)
	    {
	      if (gSystem->mkdir(dirName.c_str())==0) std::cout<<"Created directory "<<dirName<<std::endl;

	      m_outFiles[dirName]=new TFile((dirName+"/out.root").c_str(),"RECREATE");

	      std::vector<TH1F*>* histVect_tmp=new std::vector<TH1F*>();
	      for (int i=0;i<nCentCuts;i++)
		{
		  histVect_tmp->push_back(new TH1F(("h_jtpt"+centNames[i]).c_str(),("Jet pt ["+dirName+"];jtpt").c_str(),100,100,300));
		}
	      m_histVectors[dirName]=histVect_tmp;
	    }

	  TTree* t=(TTree*)inputFile->Get((dirName+"/"+treeName).c_str());
	  if (t==0)
	    {
	      std::cout<<"ERROR: Tree "<<treeName<<" not found in TDirectoryFile "<<dirName<<std::endl;
	      continue;
	    }
	  
	  const int nMaxJets=500;

	  Int_t nref;
	  TBranch* b_nref;
	  t->SetBranchAddress("nref", &nref, &b_nref);

	  Float_t jtpt[nMaxJets];
	  TBranch* b_jtpt;
	  t->SetBranchAddress("jtpt", jtpt, &b_jtpt);

	  std::cout<<"   Filling histograms"<<std::endl;

	  Long64_t nentries=t->GetEntriesFast();
	  for (Long64_t jentry=0;jentry<nentries;jentry++)
	    {
	      Long64_t ientry=t->LoadTree(jentry);
	      if (ientry<0)
		{
		  std::cout<<"Something is wrong. Filling failed"<<std::endl;
		  break;
		}

	      t->GetEntry(jentry);
	      HiTree->GetEntry(jentry);

	      if (hiBin<0&&hi) std::cout<<"WARNING: Some or all events have undefined centrality. Double check that this is actual hi data"<<std::endl;

	      std::vector<TH1F*>* histVector=m_histVectors[dirName];
	      for (int i=0;i<nCentCuts;i++)
		{
		  if (hiBin>=centCuts[i]&&hiBin<centCuts[i+1])
		    {
		      m_evtCounts[dirName][i]++;
		      for (int j=0;j<nref;j++)
			{
			  histVector->at(i)->Fill(jtpt[j]);
			}
		    }
		}
	    }
	}

      nFiles++;
      inputFile->Close();
    }

  std::cout<<std::endl<<"All files processed. Generating output for:"<<std::endl;

  TFile* shell=new TFile("tmp.root","RECREATE");
  CustomCanvas* c1=new CustomCanvas("c1", "c1", 750, 750);
  for (auto kvp : m_histVectors)
    {
      std::cout<<"  "<<kvp.first<<std::endl;

      std::vector<TH1F*>* hV=kvp.second;

      TLegend* h_legend=new TLegend(0.1,0.7,0.48,0.9);
      h_legend->SetFillStyle(0);
      h_legend->SetBorderSize(0);

      gStyle->SetOptStat("");
      long maxCount=0;
      for(int i=0;i<nCentCuts;i++)
	{
	  long hMax=hV->at(i)->GetBinContent(hV->at(i)->GetMaximumBin());
	  if (hMax>maxCount) maxCount=hMax;
	}
      maxCount*=1.2;
      for(int i=0;i<nCentCuts;i++)
	{
	  TH1F* h_tmp=hV->at(i);
	  h_tmp->SetMaximum(maxCount);
	  h_tmp->SetMarkerColor(kP->getColor(i));
	  h_tmp->SetMarkerStyle(styleList.at(i));
	  if (m_evtCounts[kvp.first][i]!=0) h_tmp->Scale((double)1/m_evtCounts[kvp.first][i]);
	  
	  h_legend->AddEntry(h_tmp,(centTitles[i].substr(2)).c_str(),"p");
	  if (i==0) h_tmp->Draw("p");
	  else h_tmp->Draw("p same");
	}
      h_legend->Draw();
      
      c1->SaveAs((kvp.first+"/spectraPlot.png").c_str());

      gPad->SetLogx();
      for(int i=0;i<nCentCuts;i++)
	{
	  TH1F* h_tmp=hV->at(i);

	  if (i==0) h_tmp->Draw("p");
	  else h_tmp->Draw("p same");
	}
      h_legend->Draw();

      c1->SaveAs((kvp.first+"/spectraPlot_log.png").c_str());

      gPad->SetLogx(0);

      m_outFiles[kvp.first]->Write();
      m_outFiles[kvp.first]->Close();
    }
  std::cout<<std::endl<<"Generating slides"<<std::endl;
  TMVASlides(new std::vector<std::vector<std::string>*>{c1->GetPointer()},slideName,2);
  delete c1;
  delete shell;

  std::cout<<std::endl<<"Finished"<<std::endl<<"Number of files processed: "<<nFiles<<std::endl;
}
