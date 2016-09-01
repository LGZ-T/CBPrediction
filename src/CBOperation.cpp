#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/ValueSymbolTable.h>

#define NUMBLOCKS 20000

using namespace llvm;

enum inst_type {reg_inst, incall_inst, /*mpicall_inst,*/ libcall_inst, notfound};    
std::string outfunc, mainfunc, skifunc;
unsigned long long cbid; 
GlobalVariable *Count, *Cycle;
bool use_opt;

Constant *Inc, *GC;
Type *ATy, *int64ty;
Function *GetCycle;


void insertCBCount(IRBuilder<>& Builder)
{
    if(cbid>=NUMBLOCKS) { errs() << "excede the maxmium code blocs(20000)\n"; exit(1); }
    LLVMContext &Context = Inc->getContext();
    std::vector<Constant*> Indices(2);
    Indices[0] = Constant::getNullValue(Type::getInt32Ty(Context));
    Indices[1] = ConstantInt::get(Type::getInt32Ty(Context),cbid);
    Constant *ElementPtr = ConstantExpr::getGetElementPtr(Count,Indices);

    Value *OldVal = Builder.CreateLoad(ElementPtr, "OldBlockCounter");
    Value *NewVal = Builder.CreateAdd(OldVal,
            Builder.CreateZExtOrBitCast(Inc,Type::getInt64Ty(Context)),
            "NewBlockCounter");
    Builder.CreateStore(NewVal, ElementPtr);
}


void insertCBCycle(IRBuilder<>& Builder, bool isfirst)
{
    if(cbid>=NUMBLOCKS) { errs() << "excede the maxmium code blocs(20000)\n"; exit(1); }
    static CallInst *cycle1, *cycle2;
    if(isfirst)
    {
        cycle1 = Builder.CreateCall(GetCycle,"");
        return;
    }
    cycle2 = Builder.CreateCall(GetCycle,"");
    Value *sub = Builder.CreateSub(cycle2,cycle1);
    std::vector<Constant*> Indices(2);
    LLVMContext &Context = sub->getContext();
    Indices[0] = Constant::getNullValue(Type::getInt32Ty(Context));
    Indices[1] = ConstantInt::get(Type::getInt32Ty(Context),cbid);
    Constant *ElementPtr = ConstantExpr::getGetElementPtr(Cycle,Indices);
    LoadInst *loadinst = Builder.CreateLoad(ElementPtr,"");
    Value *add = Builder.CreateAdd(loadinst,sub);
    Builder.CreateStore(add,ElementPtr);
    return;
}

/*
 * if the instruction is not a call, then return reg_inst;
 * if the instruction is a call inst, but i cannot get the callee(see llvm document for getCalledFunction),
 * return libcall_inst;
 * if the instruction is a call inst, and the callee is defined in the module.return incall_inst;
 */
inst_type  my_inst_type(Instruction *inst,Module &M)
{
    CallInst *callfunc = (CallInst *)inst;
    if(std::string(inst->getOpcodeName())!="call") return reg_inst;
    if(callfunc->getCalledFunction()==nullptr) 
    {
        std::string inststr;
        raw_string_ostream inststream(inststr); 
        callfunc->getCalledValue()->print(inststream);
        std::string temp = inststream.str();
        std::size_t pos = temp.find("@"), length=0;

        //this could happen when the callee is a func pointer
        //for now, we just consider it as a incall_inst
        if(pos==std::string::npos){ errs() << "callee is not found\n"; return incall_inst; }
        ++pos;
        while(temp[pos]!=' ')
        {
            ++length;
            ++pos;
        }
        Function *func = M.getFunction(temp.substr(pos-length,length));
        if(func==nullptr)
        {
            errs() << "callee's name can not be found in the symbol table: "
                << temp.substr(pos-length,length) << "\n";
            return  notfound;
        }
        if(func->isDeclaration()) return libcall_inst;
        else return incall_inst;
    }

    if(callfunc->getCalledFunction()->isDeclaration()) return libcall_inst;
    return incall_inst;
}

BasicBlock::iterator PreProcessBB(BasicBlock &bb, Function &f, Module &M, 
        LLVMContext &Context, IRBuilder<>& Builder)
{
    int phiinstcount = 0;
    //skip all phi instructions
    for(BasicBlock::iterator tbegin=bb.begin();;++tbegin)
    {
        Instruction &tinst = *tbegin;
        if(std::string(tinst.getOpcodeName())=="phi") 
            ++phiinstcount;
        else break;
    }

    //get the first and last instruction before which we can insert our instruction
    BasicBlock::iterator itet = bb.getFirstInsertionPt();
    Instruction *first = &*itet;
    Instruction *last = &*(--(bb.end()));

    //if the last instruction is the ret instruction of the main function, then we
    //insert a call outinfo_xxxx() instruction to write info into files
    if(std::string(f.getName())==mainfunc && 
            std::string(last->getOpcodeName())=="ret")
    {
        Constant *FuncEntry = M.getOrInsertFunction(outfunc, Type::getVoidTy(Context),nullptr,nullptr);
        CallInst::Create(FuncEntry,"",last);
    }

    //if there are less than 3 instructions execpt for phi instruction, we ignore this code block,
    //except for one situation:
    //and there is no call instruction, we ignore these code blocks
    if(first==last) return bb.end();
    if(bb.size()-phiinstcount <= 2)
    {
        if(std::string(first->getOpcodeName())=="call" && 
                my_inst_type(first,M)!=incall_inst)
        {
            Builder.SetInsertPoint(first);
            ++cbid;
            insertCBCount(Builder);
            if(outfunc=="outinfo_cbcycle")
            {
                insertCBCycle(Builder,true);
                Builder.SetInsertPoint(last);
                insertCBCycle(Builder,false);
            }
        }
        return bb.end();
    }

    while(my_inst_type((Instruction*)itet,M)==incall_inst) { ++itet; }
    return itet;
}

void SplitBB(BasicBlock::iterator itet, BasicBlock &bb, Module &M, 
        IRBuilder<>& Builder)
{
    Instruction *first = (Instruction *)itet;
    Instruction *bbfirst = first;
    Instruction *bblast = &*(--(bb.end()));
    int continue_inst = 0;
    bool hasinserted = false, varied_cb = false;
    while(((Instruction*)itet)!=bblast)
    {
        inst_type temp_inst_type = my_inst_type((Instruction*)itet,M);
        if(temp_inst_type==reg_inst)
        {
            ++continue_inst;
        }
        else if(temp_inst_type==libcall_inst)
        {
            continue_inst=5;
            varied_cb = true;
        }
        else
        {
            if(continue_inst>=3)
            {
                ++cbid;
                if(!hasinserted)
                {
                    Builder.SetInsertPoint(bbfirst);
                    insertCBCount(Builder);
                    hasinserted = true;
                }
                if(outfunc=="outinfo_cbcycle" && 
                        (!use_opt || (use_opt && varied_cb)))
                {
                    Builder.SetInsertPoint(first);
                    insertCBCycle(Builder,true);
                    Builder.SetInsertPoint(itet);
                    insertCBCycle(Builder,false);
                    varied_cb = false;
                }
            }
            ++itet;
            while(my_inst_type((Instruction*)itet,M)==incall_inst) { ++itet; }
            if(((Instruction*)itet)==bblast) { continue; }
            first = (Instruction*)itet;
            continue_inst = 0;
        }
        ++itet;
        if(((Instruction*)itet)==bblast)
        {
            if(continue_inst>=3)
            {
                ++cbid;
                if(!hasinserted)
                {
                    Builder.SetInsertPoint(bbfirst);
                    insertCBCount(Builder);
                    hasinserted = true;
                }
                if(outfunc=="outinfo_cbcycle" && 
                        (!use_opt || (use_opt && varied_cb)))
                {
                    Builder.SetInsertPoint(first);
                    insertCBCycle(Builder,true);
                    Builder.SetInsertPoint(bblast);
                    insertCBCycle(Builder,false);
                    varied_cb = false;
                }

            }
        }
    }
}

void process_module(Module &M, std::string out, bool isopt)
{
    LLVMContext &Context = M.getContext();
    IRBuilder<> Builder(Context);
    cbid = -1;
    use_opt = isopt;

    ATy = ArrayType::get(Type::getInt64Ty(Context),NUMBLOCKS);

    Count = new GlobalVariable(M, ATy, false,
            GlobalVariable::ExternalLinkage, Constant::getNullValue(ATy),
            "BlockPredCount");
    Cycle = new GlobalVariable(M, ATy, false,
            GlobalVariable::ExternalLinkage, Constant::getNullValue(ATy),
            "BlockPredCycle");

    Inc = ConstantInt::get(Type::getInt32Ty(Context),1);
    int64ty = Type::getInt64Ty(Context);
    GC = M.getOrInsertFunction("llvm.readcyclecounter",int64ty,nullptr);
    GetCycle = cast<Function>(GC);

    mainfunc="MAIN__";
    skifunc="main";
    if(M.getFunction("MAIN__")==nullptr)
    {
        mainfunc = "main";
        skifunc = "";
    }
    outfunc = out;

    for(Module::iterator itefunc=M.begin(),endfunc=M.end();itefunc!=endfunc;++itefunc)
    {
        Function &f = *itefunc;
        if(f.getName()==skifunc) continue;
        if(f.isDeclaration()) continue;
        for(Function::iterator itebb=f.begin(),endbb=f.end();itebb!=endbb;++itebb)
        {
            BasicBlock &bb = *itebb;
            BasicBlock::iterator itet;

            if((itet=PreProcessBB(bb,f,M,Context,Builder))==bb.end()) continue;

            SplitBB(itet,bb,M,Builder);
        }
    }
}
