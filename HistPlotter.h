#ifndef HISTPLOTTER_H
#define HISTPLOTTER_H
#include <TCanvas.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TStyle.h>
#include <iostream>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TCutG.h>
#include <signal.h>
#include <cstdlib>
#include <utility>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <set>

class HistPlotter {
private:
    std::unordered_map<std::string,TObject*> oMap; //list of all objects held by the plotter class
    std::unordered_map<std::string,TObject*> cutsMap;
    std::set<std::string> folderList;
    std::unordered_map<TObject*,std::string> foldersForObjects;
    TFile *ofile=nullptr;

public:
    HistPlotter(std::string outfile);
    void FlushToDisk(); //write all objects to file before closing
    void PrintObjects();
    void FillHistograms() {} //get all objects inheriting from th1 and th2 here
    void ReadCuts(std::string);
    TCutG* FindCut(std::string cut) {
        return static_cast<TCutG*>(cutsMap.at(cut));
    }
    //Smart functions that create a new histogram if it doesn't exist.
    void Fill1D(const std::string& name,int nbinsx, float xlow, float xhigh, float value);
    void Fill2D(const std::string& name,int nbinsx, float xlow, float xhigh
                ,int nbinsy, float ylow, float yhigh, float valuex, float valuey);
    void Fill1D(const std::string& name,int nbinsx, float xlow, float xhigh, float value, const std::string& folder);
    void Fill2D(const std::string& name,int nbinsx, float xlow, float xhigh
                ,int nbinsy, float ylow, float yhigh, float valuex, float valuey, const std::string& folder);
    //TObject* findObject(std::string key);
};
#endif
