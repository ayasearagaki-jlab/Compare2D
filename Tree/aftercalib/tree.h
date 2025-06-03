//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Jun  3 09:59:52 2025 by ROOT version 6.32.02
// from TTree tree/calibtree
// found on file: shibataDATA.root
//////////////////////////////////////////////////////////

#ifndef tree_h
#define tree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class tree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Double_t        E1;
   Double_t        E_narrow1;
   Double_t        E2;
   Double_t        E_narrow2;
   Double_t        E3;
   Double_t        E_narrow3;
   Double_t        E4;
   Double_t        E_narrow4;
   Double_t        E5;
   Double_t        E_narrow5;
   Double_t        E6;
   Double_t        E_narrow6;
   Double_t        diff_TRG;
   Double_t        diff_TDC1minusTDC4;
   Double_t        diff_TDC1minusTDC1high;
   Double_t        diff_TDC1minusTDC2;
   Double_t        diff_TDC1minusTDC3;
   Double_t        diff_TDC1minusTDC5;
   Double_t        diff_TDC1minusTDC6;
   Double_t        diff_TDC3minusTDC6;
   Double_t        diff_TDC3minusTDC3high;
   Double_t        diff_TDC3minusTDC1;
   Double_t        diff_TDC3minusTDC2;
   Double_t        diff_TDC3minusTDC4;
   Double_t        diff_TDC3minusTDC5;
   Double_t        diff_TDC5minusTDC2;
   Double_t        diff_TDC5minusTDC5high;
   Double_t        diff_TDC5minusTDC1;
   Double_t        diff_TDC5minusTDC3;
   Double_t        diff_TDC5minusTDC4;
   Double_t        diff_TDC5minusTDC6;

   // List of branches
   TBranch        *b_E1;   //!
   TBranch        *b_E_narrow1;   //!
   TBranch        *b_E2;   //!
   TBranch        *b_E_narrow2;   //!
   TBranch        *b_E3;   //!
   TBranch        *b_E_narrow3;   //!
   TBranch        *b_E4;   //!
   TBranch        *b_E_narrow4;   //!
   TBranch        *b_E5;   //!
   TBranch        *b_E_narrow5;   //!
   TBranch        *b_E6;   //!
   TBranch        *b_E_narrow6;   //!
   TBranch        *b_diff_TRG;   //!
   TBranch        *b_diff_TDC1minusTDC4;   //!
   TBranch        *b_diff_TDC1minusTDC1high;   //!
   TBranch        *b_diff_TDC1minusTDC2;   //!
   TBranch        *b_diff_TDC1minusTDC3;   //!
   TBranch        *b_diff_TDC1minusTDC5;   //!
   TBranch        *b_diff_TDC1minusTDC6;   //!
   TBranch        *b_diff_TDC3minusTDC6;   //!
   TBranch        *b_diff_TDC3minusTDC3high;   //!
   TBranch        *b_diff_TDC3minusTDC1;   //!
   TBranch        *b_diff_TDC3minusTDC2;   //!
   TBranch        *b_diff_TDC3minusTDC4;   //!
   TBranch        *b_diff_TDC3minusTDC5;   //!
   TBranch        *b_diff_TDC5minusTDC2;   //!
   TBranch        *b_diff_TDC5minusTDC5high;   //!
   TBranch        *b_diff_TDC5minusTDC1;   //!
   TBranch        *b_diff_TDC5minusTDC3;   //!
   TBranch        *b_diff_TDC5minusTDC4;   //!
   TBranch        *b_diff_TDC5minusTDC6;   //!

   tree(TTree *tree=0);
   virtual ~tree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual bool     Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef tree_cxx
tree::tree(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("shibataDATA.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("shibataDATA.root");
      }
      f->GetObject("tree",tree);

   }
   Init(tree);
}

tree::~tree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t tree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t tree::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void tree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("E1", &E1, &b_E1);
   fChain->SetBranchAddress("E_narrow1", &E_narrow1, &b_E_narrow1);
   fChain->SetBranchAddress("E2", &E2, &b_E2);
   fChain->SetBranchAddress("E_narrow2", &E_narrow2, &b_E_narrow2);
   fChain->SetBranchAddress("E3", &E3, &b_E3);
   fChain->SetBranchAddress("E_narrow3", &E_narrow3, &b_E_narrow3);
   fChain->SetBranchAddress("E4", &E4, &b_E4);
   fChain->SetBranchAddress("E_narrow4", &E_narrow4, &b_E_narrow4);
   fChain->SetBranchAddress("E5", &E5, &b_E5);
   fChain->SetBranchAddress("E_narrow5", &E_narrow5, &b_E_narrow5);
   fChain->SetBranchAddress("E6", &E6, &b_E6);
   fChain->SetBranchAddress("E_narrow6", &E_narrow6, &b_E_narrow6);
   fChain->SetBranchAddress("diff_TRG", &diff_TRG, &b_diff_TRG);
   fChain->SetBranchAddress("diff_TDC1minusTDC4", &diff_TDC1minusTDC4, &b_diff_TDC1minusTDC4);
   fChain->SetBranchAddress("diff_TDC1minusTDC1high", &diff_TDC1minusTDC1high, &b_diff_TDC1minusTDC1high);
   fChain->SetBranchAddress("diff_TDC1minusTDC2", &diff_TDC1minusTDC2, &b_diff_TDC1minusTDC2);
   fChain->SetBranchAddress("diff_TDC1minusTDC3", &diff_TDC1minusTDC3, &b_diff_TDC1minusTDC3);
   fChain->SetBranchAddress("diff_TDC1minusTDC5", &diff_TDC1minusTDC5, &b_diff_TDC1minusTDC5);
   fChain->SetBranchAddress("diff_TDC1minusTDC6", &diff_TDC1minusTDC6, &b_diff_TDC1minusTDC6);
   fChain->SetBranchAddress("diff_TDC3minusTDC6", &diff_TDC3minusTDC6, &b_diff_TDC3minusTDC6);
   fChain->SetBranchAddress("diff_TDC3minusTDC3high", &diff_TDC3minusTDC3high, &b_diff_TDC3minusTDC3high);
   fChain->SetBranchAddress("diff_TDC3minusTDC1", &diff_TDC3minusTDC1, &b_diff_TDC3minusTDC1);
   fChain->SetBranchAddress("diff_TDC3minusTDC2", &diff_TDC3minusTDC2, &b_diff_TDC3minusTDC2);
   fChain->SetBranchAddress("diff_TDC3minusTDC4", &diff_TDC3minusTDC4, &b_diff_TDC3minusTDC4);
   fChain->SetBranchAddress("diff_TDC3minusTDC5", &diff_TDC3minusTDC5, &b_diff_TDC3minusTDC5);
   fChain->SetBranchAddress("diff_TDC5minusTDC2", &diff_TDC5minusTDC2, &b_diff_TDC5minusTDC2);
   fChain->SetBranchAddress("diff_TDC5minusTDC5high", &diff_TDC5minusTDC5high, &b_diff_TDC5minusTDC5high);
   fChain->SetBranchAddress("diff_TDC5minusTDC1", &diff_TDC5minusTDC1, &b_diff_TDC5minusTDC1);
   fChain->SetBranchAddress("diff_TDC5minusTDC3", &diff_TDC5minusTDC3, &b_diff_TDC5minusTDC3);
   fChain->SetBranchAddress("diff_TDC5minusTDC4", &diff_TDC5minusTDC4, &b_diff_TDC5minusTDC4);
   fChain->SetBranchAddress("diff_TDC5minusTDC6", &diff_TDC5minusTDC6, &b_diff_TDC5minusTDC6);
   Notify();
}

bool tree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return true;
}

void tree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t tree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef tree_cxx
