#include <llvm/Pass.h>
#include <llvm/IR/Module.h>
#include "CBOperation.h"
//#include <llvm/Analysis/LoopInfo.h>
//#include <llvm/Analysis/LoopPass.h>
using namespace llvm;

namespace{
    struct CBCycle:public ModulePass{
        static char ID;
        
        CBCycle():ModulePass(ID) {}

        bool runOnModule(Module &M) override
        {
            /* this is for getting loop info
            for(Module::iterator itefunc=M.begin(),endfunc=M.end();itefunc!=endfunc;++itefunc)
            {
                Function &f = *itefunc;
                if(f.isDeclaration()) continue;
                errs() << "in func " << f.getName() << ":\n";
                LoopInfo &loopinfo = getAnalysis<LoopInfo>(f);
                for(auto iteloop=loopinfo.begin(),endloop=loopinfo.end();iteloop!=endloop;++iteloop)
                {
                    Loop *loop = *iteloop;
                    unsigned loopdepth = loop->getLoopDepth();
                    errs() << "loop depth: " << loopdepth << "##";
                    if(loop->getCanonicalInductionVariable()!=nullptr)
                        errs() << "ok##" << *(loop->getCanonicalInductionVariable());
                    errs() << "\n";
                }
            }*/
            process_module(M, "outinfo_cbcycle", false);
            return true;
        }

        /*void getAnalysisUsage(AnalysisUsage &AU) const override
        {
            AU.addRequired<LoopInfo>();
            AU.setPreservesAll();
        }*/
    };
}

char CBCycle::ID = 0;
static RegisterPass<CBCycle> X("CBCycle","get the execute time of main basic block",false,false);
