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

int TMVASlides(std::vector<std::vector<std::string>*>* hvv)
{
  /*  std::vector<std::string> hists_list={"h_EScale_cent0_pt30.png",
				       "h_EScale_cent0_pt50.png",
				       "h_EScale_cent0_pt70.png",
				       "h_EScale_cent10_pt30.png",
				       "h_EScale_cent10_pt50.png",
				       "h_EScale_cent10_pt70.png",
				       "h_EScale_cent30_pt30.png",
				       "h_EScale_cent30_pt50.png",
				       "h_EScale_cent30_pt70.png",
				       "h_EScale_cent50_pt30.png",
				       "h_EScale_cent50_pt50.png",
				       "h_EScale_cent50_pt70.png",
				       "h_EScale_cent70_pt30.png",
				       "h_EScale_cent70_pt50.png",
				       "h_EScale_cent70_pt70.png"};
  */
  const std::string outTexFileName = "hists_test/slides.tex";
  std::ofstream fileTex(outTexFileName.c_str());
  
  fileTex << "\\RequirePackage{xspace}" << std::endl;
  fileTex << "\\RequirePackage{amsmath}" << std::endl;
  fileTex << std::endl;
  
  fileTex << "\\documentclass[xcolor=dvipsnames]{beamer}" << std::endl;
  fileTex << "\\usetheme{Warsaw}" << std::endl;
  fileTex << "\\setbeamercolor{structure}{fg=NavyBlue!90!NavyBlue}" << std::endl;
  fileTex << "\\setbeamercolor{footlinecolor}{fg=white,bg=lightgray}" << std::endl;
  fileTex << "\\newcommand{\\pt}{\\ensuremath{p_{\\mathrm{T}}}\\xspace}" << std::endl;
  fileTex << "\\setbeamersize{text margin left=5pt,text margin left=5pt,text margin right=5pt}" << std::endl;
  fileTex << std::endl;

  fileTex << "\\setbeamertemplate{frametitle}" << std::endl;
  fileTex << "{" << std::endl;
  fileTex << "  \\nointerlineskip" << std::endl;
  fileTex << "  \\begin{beamercolorbox}[sep=0.3cm, ht=1.8em, wd=\\paperwidth]{frametitle}" << std::endl;
  fileTex << "    \\vbox{}\\vskip-2ex%" << std::endl;
  fileTex << "    \\strut\\insertframetitle\\strut" << std::endl;
  fileTex << "    \\vskip-0.8ex%" << std::endl;
  fileTex << "  \\end{beamercolorbox}" << std::endl;
  fileTex << "}" << std::endl;

  fileTex << std::endl;

  fileTex << "\\setbeamertemplate{footline}{%" << std::endl;
  fileTex << "  \\begin{beamercolorbox}[sep=.8em,wd=\\paperwidth,leftskip=0.5cm,rightskip=0.5cm]{footlinecolor}" << std::endl;
  fileTex << "    \\hspace{0.3cm}%" << std::endl;
  fileTex << "    \\hfill\\insertauthor \\hfill\\insertpagenumber" << std::endl;
  fileTex << "  \\end{beamercolorbox}%" << std::endl;
  fileTex << "}" << std::endl;

  fileTex << std::endl;
  
  fileTex << "\\setbeamertemplate{navigation symbols}{}" << std::endl;
  fileTex << "\\setbeamertemplate{itemize item}[circle]" << std::endl;
  fileTex << "\\setbeamertemplate{itemize subitem}[circle]" << std::endl;
  fileTex << "\\setbeamertemplate{itemize subsubitem}[circle]" << std::endl;
  fileTex << "\\setbeamercolor{itemize item}{fg=black}" << std::endl;
  fileTex << "\\setbeamercolor{itemize subitem}{fg=black}" << std::endl;
  fileTex << "\\setbeamercolor{itemize subsubitem}{fg=black}" << std::endl;

  fileTex << std::endl;

  fileTex << "\\definecolor{links}{HTML}{00BFFF}" << std::endl;
  fileTex << "\\hypersetup{colorlinks,linkcolor=,urlcolor=links}" << std::endl;

  fileTex << std::endl;

  fileTex << "\\author[CM]{Placeholder}" << std::endl;
  fileTex << "\\begin{document}" << std::endl;

  fileTex << std::endl;
  /*
  fileTex << "\\begin{frame}" << std::endl;
  fileTex << "\\frametitle{\\centerline{Sample Validation (" << date->GetYear() << "." << date->GetMonth() << "." << date->GetDay() << ")}}" << std::endl;
  fileTex << " \\begin{itemize}" << std::endl;
  fileTex << "  \\fontsize{8}{8}\\selectfont" << std::endl;
  fileTex << "  \\item{" << inFile1TexStr << "}" << std::endl;
  fileTex << "  \\item{" << inFile2TexStr << "}" << std::endl;
  fileTex << " \\end{itemize}" << std::endl;
  fileTex << "\\end{frame}" << std::endl;
  */
  for (std::vector<std::string>* hists_list : *hvv)
    {
      for(unsigned int i = 0; i < hists_list->size(); ++i){
	/*std::string varStr = listOfVar.at(i);
	  std::string newVarStr;
	  while(varStr.find("_") != std::string::npos){
	  newVarStr = newVarStr + varStr.substr(0, varStr.find("_"));
	  newVarStr = newVarStr + "\\_";
	  varStr.replace(0, varStr.find("_")+1, "");
	  }
	  newVarStr = newVarStr + varStr;
	*/
	fileTex << std::endl;
	fileTex << "\\begin{frame}" << std::endl;
	//    fileTex << "\\frametitle{\\centerline{" << newVarStr << "}}" << std::endl;
	fileTex << "\\begin{center}" << std::endl;
	fileTex << "\\includegraphics[width=0.8\\textwidth]{" << hists_list->at(i) << "}" << std::endl;
	fileTex << "\\end{center}" << std::endl;
	/*    fileTex << "\\begin{itemize}" << std::endl;
	      fileTex << "\\fontsize{8}{8}\\selectfont" << std::endl;
	      fileTex << "\\item{" << newVarStr << "}" << std::endl;
	      fileTex << "\\end{itemize}" << std::endl;*/
	fileTex << "\\end{frame}" << std::endl;
      }
    }
  
  fileTex << std::endl;
  fileTex << "\\end{document}" << std::endl;

  fileTex.close();

  return 0;
}
/*
int main(int argc, char* argv[])
{
  if(argc != 3 && argc != 4){
    std::cout << "Usage ./doComparison.exe <inFileName1> <inFileName2> <outFileName-optional>" << std::endl;
    return 1;
  }

  int retVal = 0;
  if(argc == 3) retVal += doComparison(argv[1], argv[2]);
  else if(argc == 4) retVal += doComparison(argv[1], argv[2], argv[3]);
  return retVal;
  }*/
