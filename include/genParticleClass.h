#ifndef GENPARTICLECLASS_H
#define GENPARTICLECLASS_H

#include <vector>

class genParticleClass{
 public:  
  std::vector<float>* pt_p=0;
  std::vector<float>* eta_p=0;
  std::vector<float>* phi_p=0;
  std::vector<int>* chg_p=0;
  std::vector<int>* pdg_p=0;
};

#endif
