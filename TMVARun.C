#include "TMVAPlot.C"
#include "TMVARegression.C"

void TMVARun()
{
  const int nCentCuts=4;
  std::string titles_cent[]={", Cent 0-30",", Cent 30-50",", Cent 50-70",", Cent 70-100"};
  std::string names_cent[]={"_cent0","_cent30","_cent50","_cent70"};
  std::string cuts_cent[]={"&& hiBin<60","&& hiBin>=60 && hiBin<100","&& hiBin>=100 && hiBin<140","&& hiBin>=140"};
  
  /*  const int nPtCuts=3;
  std::string titles_pt[]={", pt 30-50",", pt 50-70",", pt 70-100"};
  std::string names_pt[]={"_pt30","_pt50","_pt70"};
  std::string cuts_pt[]={"&& refpt>30 && refpt<50","&& refpt>50 && refpt<70","&& refpt>70 && refpt<100"};*/

  for (int i=0;i<nCentCuts;i++){
    TMVARegression("abs(jteta)<2 && jtpt>20 && refpt>0 "+cuts_cent[i]);
    
    TMVAPlot t;
    t.Loop(titles_cent[i],names_cent[i]);
  }
}
