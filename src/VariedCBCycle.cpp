#include <llvm/Pass.h>
#include <llvm/IR/Module.h>
#include "CBOperation.h"
//#include <llvm/Analysis/LoopInfo.h>
//#include <llvm/Analysis/LoopPass.h>
using namespace llvm;

namespace{
    struct VariedCBCycle:public ModulePass{
        static char ID;
        
        VariedCBCycle():ModulePass(ID) {}

        bool runOnModule(Module &M) override
        {
            process_module(M, "outinfo_cbcycle", true);
            return true;
        }
    };
}

char VariedCBCycle::ID = 0;
static RegisterPass<VariedCBCycle> X("VaryCBCyc","get the execute cycle of non-constant code block",false,false);
