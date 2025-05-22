#include "HistPlotter.h"

HistPlotter::HistPlotter(std::string outfile) {
    ofile = new TFile(outfile.c_str(),"recreate");
}

void HistPlotter::FlushToDisk() {
    /*
        void HistPlotter::FlushToDisk()
        Function that can be used at any point to exit smoothly by saving all ROOT objects in memory
        to the output file before closing it. Obeys the binding of histograms to separate folders, if so specified.
    */
    if(ofile->IsZombie() || !ofile) {
        std::cerr << "Output file is zombie, finishing up without writing to disk!" << std::endl;
        return;
    }
    for(auto it=oMap.begin(); it!=oMap.end(); it++ ) {
        //omap maps: name(first) to object address(second).
        // foldersForObjects maps: object address(first) to foldername(second)
        auto result = foldersForObjects.find(it->second); //returns <TObject* histogram,std::string foldername> pair if found
        if(result!=foldersForObjects.end()) { //we try to create folder if needed and cd to it
            ofile->mkdir(result->second.c_str(),"",kTRUE); // args: name, title, returnExistingDirectory
            ofile->cd(result->second.c_str());
        } else {
            ofile->cd(); //toplevel for all default histograms. Default setting
        }
        it->second->Write();
    }

    //Create a directory for all cuts, and save all cuts in them
    ofile->mkdir("gCUTS","",kTRUE);
    ofile->cd("gCUTS");
    for(auto it=cutsMap.begin(); it!=cutsMap.end(); it++) {
        (static_cast<TNamed*>(it->second))->SetName(it->first.c_str());
        it->second->Write();
    }
    ofile->Close();
}

void HistPlotter::Fill1D(const std::string& name, int nbinsx, float xlow, float xhigh, float value) {
    /*
        void HistPlotter::Fill1D(std::string name, int nbinsx, float xlow, float xhigh, float value)
      -  Creates a TH1F in memory with name 'name' if it doesn't exist, and fills it with valuex, valuey
      -  Writes present state to disk and fails with return value -1 if the name clashes with another object that's not of type TH1*
    */
    auto result = oMap.find(name); //result is an iterator
    if(result==oMap.end()) {
        TH1F* temp1D = new TH1F(name.c_str(), name.c_str(), nbinsx, xlow, xhigh);
        oMap.insert(std::make_pair(name,static_cast<TObject*>(temp1D)));
    }
    //Check if the string 'name' maps to a 1D hist. If there's any other object by this name raise issue
    if(!oMap.at(name)->InheritsFrom("TH1F")) {
        std::cerr << "Object " << name << " refers to something other than a TH1*, not filling it hence!" << std::endl;
        std::cerr << "Abort.." << std::endl;
        FlushToDisk();
        exit(-1);
    }
    static_cast<TH1F*>(oMap.at(name))->Fill(value);
}

void HistPlotter::Fill1D(const std::string& name, int nbinsx, float xlow, float xhigh, float value, const std::string& foldername="") {
    /*
        void HistPlotter::Fill1D(std::string name, int nbinsx, float xlow, float xhigh, float value, const std::string& foldername="")
      -  Creates a TH1F in memory with name 'name' if it doesn't exist, and fills it with valuex, valuey
      -  Writes present state to disk and fails with return value -1 if the name clashes with another object that's not of type TH1*
      -  Remembers the foldername this particular histogram maps to, if provided. If not, defaults to toplevel.
    */

    auto result = oMap.find(name); //result is an iterator
    if(result==oMap.end()) {
        TH1F* temp1D = new TH1F(name.c_str(), name.c_str(), nbinsx, xlow, xhigh);
        oMap.insert(std::make_pair(name,static_cast<TObject*>(temp1D)));
        if(foldername!="") {
            if(folderList.find(foldername)==folderList.end()) {
                folderList.insert(foldername);
            }
            foldersForObjects.insert(std::make_pair(static_cast<TObject*>(temp1D),foldername));
        }
    }
    //Check if the string 'name' maps to a 1D hist. If there's any other object by this name raise issue
    if(!oMap.at(name)->InheritsFrom("TH1F")) {
        std::cerr << "Object " << name << " refers to something other than a TH1*, not filling it hence!" << std::endl;
        std::cerr << "Abort.." << std::endl;
        FlushToDisk();
        exit(-1);
    }
    static_cast<TH1F*>(oMap.at(name))->Fill(value);
}

void HistPlotter::Fill2D(const std::string& name, int nbinsx, float xlow, float xhigh,
                         int nbinsy, float ylow, float yhigh, float valuex, float valuey) {
    /*
        void HistPlotter::Fill2D(std::string name, int nbinsx, float xlow, float xhigh,
                                int nbinsy, float ylow, float yhigh, float valuex, float valuey)

      -  Creates a TH2F in memory with name 'name' if it doesn't exist, and fills it with valuex, valuey
      -  Writes present state to disk and fails with return value -1 if the name clashes with another object that's not of type TH2*
    */
    auto result = oMap.find(name); //result is an iterator
    if(result==oMap.end()) {
        TH2F* temp2D = new TH2F(name.c_str(), name.c_str(), nbinsx, xlow, xhigh, nbinsy, ylow, yhigh);
        oMap.insert(std::make_pair(name,static_cast<TObject*>(temp2D)));
    }
    //Check if the string 'name' maps to a 1D hist. If there's any other object by this name raise issue
    if(!oMap.at(name)->InheritsFrom("TH2F")) {
        std::cerr << "Object " << name << " refers to something other than a TH2*, not filling it hence!" << std::endl;
        std::cerr << "Abort.." << std::endl;
        FlushToDisk();
        exit(-1);
    }
    static_cast<TH1F*>(oMap.at(name))->Fill(valuex,valuey);
}

void HistPlotter::Fill2D(const std::string& name, int nbinsx, float xlow, float xhigh,
                         int nbinsy, float ylow, float yhigh, float valuex, float valuey, const std::string& foldername="") {
    /*
        void HistPlotter::Fill2D(std::string name, int nbinsx, float xlow, float xhigh,
                                int nbinsy, float ylow, float yhigh, float valuex, float valuey)

      -  Creates a TH2F in memory with name 'name' if it doesn't exist, and fills it with valuex, valuey
      -  Writes present state to disk and fails with return value -1 if the name clashes with another object that's not of type TH2*
      -  Remembers the foldername this particular histogram maps to, if provided. If not defaults to toplevel
    */
    auto result = oMap.find(name); //result is an iterator
    if(result==oMap.end()) {
        TH2F* temp2D = new TH2F(name.c_str(), name.c_str(), nbinsx, xlow, xhigh, nbinsy, ylow, yhigh);
        oMap.insert(std::make_pair(name,static_cast<TObject*>(temp2D)));
        if(foldername!="") {
            if(folderList.find(foldername)==folderList.end()) {
                folderList.insert(foldername);
            }
            foldersForObjects.insert(std::make_pair(static_cast<TObject*>(temp2D),foldername));
        }
    }
    //Check if the string 'name' maps to a 1D hist. If there's any other object by this name raise issue
    if(!oMap.at(name)->InheritsFrom("TH2F")) {
        std::cerr << "Object " << name << " refers to something other than a TH2*, not filling it hence!" << std::endl;
        std::cerr << "Abort.." << std::endl;
        FlushToDisk();
        exit(-1);
    }
    static_cast<TH1F*>(oMap.at(name))->Fill(valuex,valuey);
}

void HistPlotter::ReadCuts(std::string filename) {
    std::ifstream infile;
    infile.open(filename);
    std::string cutfilename, cutname;
    for(std::string line; std::getline(infile, line); ) {
        if(line.size()!=0 && line[0]=='#')
            ; //don't do anything with '#' lines
        else {
            std::stringstream ss(line);
            ss>>cutfilename>>cutname;

            TFile f(cutfilename.c_str());
            if(f.IsZombie()) {
                std::cerr << "Cannot open cutfile " << cutfilename << " .. skipping.." << std::endl;
                continue;
            }
            TCutG *cut = (TCutG*)(f.Get("CUTG"));
            cutsMap.insert(std::make_pair(cutname,static_cast<TObject*>(cut)));
            f.Close();
        } //else
    }//for loop
    infile.close();
}

void HistPlotter::PrintObjects() {
    /*
        void PrintObjects()
        Prints the contents of the unordered_maps oMap and cutsMap to facilitate debugging

    */
    std::cout << "Type | Name " << std::endl;
    std::cout << "---- | --------------------- " << std::endl;
    for(auto it=oMap.begin(); it!=oMap.end(); it++ ) {
        std::cout << it->second->ClassName() << " | "<< it->first << std::endl;
    }
    for(auto it=cutsMap.begin(); it!=cutsMap.end(); it++ ) {
        std::cout << it->second->ClassName() << " | "<< it->first << std::endl;
    }
    std::cout << "---- | --------------------- " << std::endl;
}
