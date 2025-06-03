#include <TFile.h>
#include <TTree.h>
#include <TSystem.h>
#include <TList.h>
#include <TString.h>
#include <iostream>

std::string type = "usshiDATA";//FIXME

void MergeTree() {
    std::string path = "/home/fermi/rshibata/Tree/aftercalib/" + type + "/";
    std::string outpath = "/home/fermi/rshibata/Tree/aftercalib/" + type + ".root";

    void* dirp = gSystem->OpenDirectory(path.c_str());
    if (!dirp) {
        std::cerr << "Directory not found: " << path << std::endl;
        return;
    }

    const char* entry;
    TList* list = new TList();

    while ((entry = gSystem->GetDirEntry(dirp))) {
        TString fname(entry);
        if (fname.EndsWith(".root")) {
            TString fullpath = path.c_str() + fname;
            std::cout << "Opening file: " << fullpath << std::endl;

            TFile* file = TFile::Open(fullpath);
            if (!file || file->IsZombie()) {
                std::cerr << "Failed to open file: " << fullpath << std::endl;
                continue;
            }

            TTree* tree = dynamic_cast<TTree*>(file->Get("calibtree"));
            if (!tree) {
                std::cerr << "TTree 'calibtree' not found in: " << fullpath << std::endl;
                file->Close();
                continue;
            }

            list->Add(tree);  
        }
    }

    gSystem->FreeDirectory(dirp);

    if (list->GetEntries() == 0) {
        std::cerr << "No trees found to merge!" << std::endl;
        return;
    }



    TFile* outfile = new TFile(outpath.c_str(), "RECREATE");
    TTree* newtree = TTree::MergeTrees(list);
    if (!newtree) {
        std::cerr << "MergeTrees failed!" << std::endl;
        return;
    }
    newtree->SetName("tree");
    newtree->Write();
    outfile->Close();

    std::cout << "Merged tree written to: " << outpath << std::endl;
}
