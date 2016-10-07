#!/bin/bash

#maybe use llc flags -mcmodel=medium or large
OPT=opt-3.5
LLC=llc-3.5
LLVMCC=clang-3.5
LLVMCXX=clang++-3.5
LLVMLINK=llvm-link-3.5
BCFILE=example
#assume our single llvm bitcode file named ${BCFILE}.bc

#1. if use iteration optimization, 
#       if the iteration variable is controled by the parameter file. then
#           we don't need to recompile the application
#       else if the iteration variable is controled by the source code. then 
#           recompile the application
#       this optimization will produce the file ${BCFILE}.optB.bc
#
#
#  ${BCFILE}.bc ----------opt iteration----------> ${BCFILE}.optB.bc
#      |                                                |
#      |                                                |
#  instrument                                       instrument
#   cb count                                         cb cycle
#      |                                                |
#     \|/                                              \|/                
#  ${BCFILE}.cbcount.bc                            ${BCFILE}.optB.cbcycle.bc--|
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
#                               ${BCFILE}.optBA.cbcycle.cbcount.bc       ${BCFILE}.optB.cbcycle.cbcount.bc
#
# ${BCFILE}.cbcount.bc:will be executed only once at a spcified processor count, say nprocx. 
#                    This file will produce execution counts of each code block of original application.
#                    This will be executed only once
# ${BCFILE}.optBA.cbcycle.cbcount.bc: this will generate execution counts of the mini benchmark's code bloks,
#                    and execution cycle of non-constant code blocks
# ${BCFILE}.optB.cbcycle.cbcount.bc: this will generate execution counts ofmini benchmark and execution counts
#                    of all code blocks, this will be executed only once.




${OPT} -load ./libCodeBlockPred.so -CBCount ${BCFILE}.bc -o ${BCFILE}.cbcount.bc
${OPT} -load ./libCodeBlockPred.so -CBCycle ${BCFILE}.optB.bc -o ${BCFILE}.optB.cbcycle.bc
${OPT} -load ./liBCodeBlockPred.so -VaryCBCyc ${BCFILE}.optB.cbcycle.bc -o ${BCFILE}.optBA.cbcycle.bc
${OPT} -load ./libCodeBlockPred.so -CBCount ${BCFILE}.optBA.cbcycle.bc -o ${BCFILE}.optBA.cbcycle.cbcount.bc
${OPT} -load ./libCodeBlockPred.so -CBCount ${BCFILE}.optB.cbcycle.bc -o ${BCFILE}.optB.cbcycle.cbcount.bc

${LLVMCC} -c -emit-llvm getbbtime.c -o getbbtime.bc

${LLVMLINK} ${BCFILE}.cbcount.bc getbbtime.bc -o ${BCFILE}.cbcount.t.bc
${LLVMLINK} ${BCFILE}.optBA.cbcycle.cbcount.bc getbbtime.bc -o ${BCFILE}.optBA.cbcycle.cbcount.t.bc
${LLVMLINK} ${BCFILE}.optB.cbcycle.cbcount.bc getbbtime.bc -o ${BCFILE}.optB.cbcycle.cbcount.t.bc

${LLC} -filetype=obj ${BCFILE}.cbcount.t.bc -o ${BCFILE}.cbcount.o
${LLC} -filetype=obj ${BCFILE}.optBA.cbcycle.cbcount.t.bc -o ${BCFILE}.optBA.cbcycle.cbcount.o
${LLC} -filetype=obj ${BCFILE}.optB.cbcycle.cbcount.t.bc -o ${BCFILE}.optB.cbcycle.cbcount.o

#finally, compile tese three .o files into the executables.
