#include <llvm/Pass.h>
#include <llvm/IR/Module.h>
#include "CBOperation.h"

using namespace llvm;

namespace{
    struct CBCount:public ModulePass{
        static char ID;
        CBCount():ModulePass(ID) {}
        
        bool runOnModule(Module &M) override
        {
            process_module(M,"outinfo_cbcount", false);
            return true;
        }
    };
}

char CBCount::ID = 0;
static RegisterPass<CBCount> X("CBCount","get the execution counts of all basic block",false,false);
