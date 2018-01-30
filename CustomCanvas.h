#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class CustomCanvas{
 private:
  TCanvas* c;
  std::vector<std::string>* histNames=new std::vector<std::string>();

 public:
  std::vector<std::string>* GetPointer()
    {
      return histNames;
    }
  void SaveAs(const char* filename="", Option_t* option="")
  {
    histNames->push_back((std::string)filename);

    c->SaveAs(filename, option);
  }
  CustomCanvas(const char* name, const char* title, Int_t ww, Int_t wh)
    {
      c=new TCanvas(name, title, ww, wh);
    }
  ~CustomCanvas()
    {
      delete c;
    }
};
