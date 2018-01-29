#include "TMVAPlot.C"
#include "TMVARegression.C"
#include "TMVASlides.C"

const int n_input=25;

void TMVARun(bool doRegression=false)
{
  std::vector<std::vector<std::string>*>* histVectorVector=new std::vector<std::vector<std::string>*>();
  std::vector<std::vector<std::string>*>* histVectorVector_train=new std::vector<std::vector<std::string>*>();
  
  const int nCentCuts=2;
  std::string titles_cent[]={", Cent 0-10",", Cent 10-30",", Cent 30-50",", Cent 50-70",", Cent 70-100"};
  std::string names_cent[]={"_cent0","_cent10","_cent30","_cent50","_cent70"};
  std::string cuts_cent[]={"&& hiBin<20","&& hiBin>=20 && hiBin<60","&& hiBin>=60 && hiBin<100","&& hiBin>=100 && hiBin<140","&& hiBin>=140"};
  
  /*  const int nPtCuts=3;
  std::string titles_pt[]={", pt 30-50",", pt 50-70",", pt 70-100"};
  std::string names_pt[]={"_pt30","_pt50","_pt70"};
  std::string cuts_pt[]={"&& refpt>30 && refpt<50","&& refpt>50 && refpt<70","&& refpt>70 && refpt<100"};*/

  for (int i=0;i<nCentCuts;i++){
    TString filename;
    if (doRegression) filename=TMVARegression("std::abs(jteta)<2 && refpt>10 "+cuts_cent[i], names_cent[i], n_input);
    else filename="/afs/cern.ch/work/s/skanaski/TMVAReg"+names_cent[i]+".root";

    TFile* file=TFile::Open(filename);

    TTree* test=(TTree*)file->Get("dataset/TestTree");
    TMVAPlot t_test(test);
    histVectorVector->push_back(t_test.Loop(titles_cent[i],names_cent[i],"hists_test/"));

    TTree* train=(TTree*)file->Get("dataset/TrainTree");
    TMVAPlot t_train(train);
    histVectorVector_train->push_back(t_train.Loop(titles_cent[i],names_cent[i],"hists_test/train/"));
  }
  
  TMVASlides(histVectorVector,"hists_test/slides_test.tex");
  TMVASlides(histVectorVector_train,"hists_test/slides_train.tex"

  TDatime* date=new TDatime();
  gSystem->Exec(("tar -czvf /afs/cern.ch/user/s/skanaski/public/TMVAHists_"+std::to_string(date->GetDate())+".tar.gz hists_test").c_str());
}
