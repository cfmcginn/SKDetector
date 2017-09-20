#ifndef PDGTOMASSINGEV_H
#define PDGTOMASSINGEV_H

#include <map>
#include <cmath>
#include <iostream>

class pdgToMassInGeV
{
 public:
  pdgToMassInGeV();
  ~pdgToMassInGeV(){}

  double getMassFromID(const int inID);

 private:
  std::map<int, double> pdgMassMap;
};

//Masses using pdg numbering scheme, taken from latest pdg particle listings
//see here for id: http://pdg.lbl.gov/2007/reviews/montecarlorpp.pdf
//see here for masses: http://pdg.lbl.gov/2017/listings/contents_listings.html


pdgToMassInGeV::pdgToMassInGeV()
{
  pdgMassMap[11] = .0005109989461;//electron
  pdgMassMap[12] = 0.;//e neutrino
  pdgMassMap[13] = .1056583745;//muon
  pdgMassMap[14] = 0.;//mu neutrino
  pdgMassMap[15] = 1776.86;//tau
  pdgMassMap[16] = 0.;//tau neutrino

  pdgMassMap[22] = 0.;//gamma
  pdgMassMap[23] = 91.1876;//Z boson
  pdgMassMap[24] = 80.385;//W boson

  pdgMassMap[111] = .1349770;//pi0
  pdgMassMap[211] = .13957061;//pi+-
  pdgMassMap[130] = .497611;//Klong
  pdgMassMap[310] = .497611;//Kshort
  pdgMassMap[321] = .493677;//K+-
  
  pdgMassMap[2112] = .9395654133;//neutron
  pdgMassMap[2212] = .9382720813;//proton
  pdgMassMap[3112] = 1.197449;//sigma-
  pdgMassMap[3122] = 1.115683;//lambda
  pdgMassMap[3222] = 1.18937;//sigma+
  pdgMassMap[3312] = 1.32171;//cascade-
  pdgMassMap[3322] = 1.31486;//cascade0
  pdgMassMap[3334] = 1.67245;//omegababy
}

double pdgToMassInGeV::getMassFromID(const int inID)
{
  if(pdgMassMap.find(std::abs(inID)) == pdgMassMap.end()){
    std::cout << "ID \'" << inID << "\' not found. Please add to \'pdgToMassInGev\'. Return -1" << std::endl;
    return -1;
  }
  return pdgMassMap[std::abs(inID)];
}

#endif
