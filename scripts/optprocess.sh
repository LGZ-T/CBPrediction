#!/bin/bash

#maybe use llc flags -mcmodel=medium or large
OPT=opt-3.5
LLC=llc-3.5
LLVMCC=clang-3.5
LLVMCXX=clang++-3.5
LLVMLINK=llvm-link-3.5
#assume our single llvm bitcode file named example.bc

#1. if use iteration optimization, 
#       if the iteration variable is controled by the parameter file. then
#           we don't need to recompile the application
#       else if the iteration variable is controled by the source code. then 
#           recompile the application
#       this optimization will produce the file example.optB.bc


#  example.bc ----------opt iteration----------> example.optB.bc
#      |                                                |
#      |                                                |
#  instrument                                       instrument
#   cb count                                         cb cycle
#      |                                                |
#     \|/                                              \|/                
#  example.cbcount.bc                            example.optB.cbcycle.bc--|
#                                                       |                 |
#                                                       |                 |
#                                                   optimization          |
#                                             non-constant code blocks    |
#                                                       |                 |
#                                                      \|/                |
#                                               exmaple.optBA.cbcycle.bc  |
#                                                       |                 |
#                                                       |                 |
#                                                       |instruct cb count|
#                                                       |                 |
#                                                      \|/               \|/
#                               example.optBA.cbcycle.cbcount.bc       example.optB.cbcycle.cbcount.bc
#
# example.cbcount.bc:will be executed only once at a spcified processor count, say nprocx. 
#                    This file will produce execution counts of each code block of original application.
#                    This will be executed only once
# example.optBA.cbcycle.cbcount.bc: this will generate execution counts of the mini benchmark's code bloks,
#                    and execution cycle of non-constant code blocks
# example.optB.cbcycle.cbcount.bc: this will generate execution counts ofmini benchmark and execution counts
#                    of all code blocks, this will be executed only once.




${OPT} -load ./libCodeBlockPred.so -CBCount example.bc -o example.cbcount.bc
${OPT} -load ./libCodeBlockPred.so -CBCycle example.optB.bc -o example.optB.cbcycle.bc
${OPT} -load ./liBCodeBlockPred.so -VaryCBCyc example.optB.cbcycle.bc -o example.optBA.cbcycle.bc
${OPT} -load ./libCodeBlockPred.so -CBCount example.optBA.cbcycle.bc -o example.optBA.cbcycle.cbcount.bc
${OPT} -load ./libCodeBlockPred.so -CBCount example.optB.cbcycle.bc -o example.optB.cbcycle.cbcount.bc

${LLVMCC} -c -emit-llvm getbbtime.c -o getbbtime.bc

${LLVMLINK} example.cbcount.bc getbbtime.bc -o example.cbcount.t.bc
${LLVMLINK} example.optBA.cbcycle.cbcount.bc getbbtime.bc -o example.optBA.cbcycle.cbcount.t.bc
${LLVMLINK} example.optB.cbcycle.cbcount.bc getbbtime.bc -o example.optB.cbcycle.cbcount.t.bc

${LLC} -filetype=obj example.cbcount.t.bc -o example.cbcount.o
${LLC} -filetype=obj example.optBA.cbcycle.cbcount.t.bc -o example.optBA.cbcycle.cbcount.o
${LLC} -filetype=obj example.optB.cbcycle.cbcount.t.bc -o example.optB.cbcycle.cbcount.o

#finally, compile tese three .o files into the executables.
