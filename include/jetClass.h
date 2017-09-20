#ifndef JETCLASS_H
#define JETCLASS_H

class jetClass{
 public:  
  const static int nMaxJets = 500;

  Int_t nref_;
  Float_t jtpt_[nMaxJets];
  Float_t jtphi_[nMaxJets];
  Float_t jteta_[nMaxJets];
  Float_t jtegfrac_[nMaxJets];
  Float_t refpt_[nMaxJets];
  Float_t refphi_[nMaxJets];
  Float_t refeta_[nMaxJets];
  Float_t ref2pt_[nMaxJets];
  Float_t ref2phi_[nMaxJets];
  Float_t ref2eta_[nMaxJets];
};

#endif
