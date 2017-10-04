#ifndef RETURNFILELISTFROMTXT_H
#define RETURNFILELISTFROMTXT_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "include/checkMakeDir.h"


bool isFileTxt(const std::string txtFileName)
{
  if(txtFileName.size() < 4) return false;
  if(txtFileName.substr(txtFileName.size()-4, 4).find(".txt") != std::string::npos) return true;
  return false;
}


std::vector<std::string> returnFileListFromTxt(const std::string txtFileName, const std::string filterStr = "")
{
  std::vector<std::string> fileList;

  if(!checkFile(txtFileName)){
    std::cout << "Given input \'" << txtFileName << "\' is not a valid file, return empty fileList" << std::endl;
    return fileList;
  }

  if(!isFileTxt(txtFileName)){
    std::cout << "Given input \'" << txtFileName << "\' is not ending in extension \'.txt\', return empty fileList" << std::endl;
    return fileList;
  }

  std::ifstream file(txtFileName.c_str());
  std::string tempStr;

  while(std::getline(file, tempStr)){
    if(tempStr.size() == 0) continue;
    if(filterStr.size() != 0 && tempStr.find(filterStr) == std::string::npos) continue;

    fileList.push_back(tempStr);
  }

  file.close();

  return fileList;
}

#endif
