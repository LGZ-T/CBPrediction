#!/bin/bash -x

mkdir -p ./bin/originexec
mkdir -p ./bin/originbc
mkdir -p ./bin/edgebc
mkdir -p ./bin/edgeexec
mkdir -p ./bin/bbtimebc
mkdir -p ./bin/bbtimeexec
mkdir -p ./bin/bbtime_count_exec

for(( i=2; i<=15; i++ ))
do
#    make sp NPROCS=$[$i*$i] CLASS=D
#    make bt NPROCS=$[$i*$i] CLASS=D
#    rm ./SP/*.o ./BT/*.o ./common/*.o
    make sp NPROCS=$[$i*$i] CLASS=D llvm=1
    make bt NPROCS=$[$i*$i] CLASS=D llvm=1
    rm ./SP/*.o ./BT/*.o ./common/*.o
done
mv ./bin/sp.D.*.bc ./bin/bt.D.*.bc ./bin/originbc
#mv ./bin/sp.D.* bt.D.* ./bin/originexec
#
#for(( i=2; i<=256; i=i*2 ))
#do
#    make cg NPROCS=$i CLASS=D
#    make ft NPROCS=$i CLASS=D
#    make lu NPROCS=$i CLASS=D
#    make is NPROCS=$i CLASS=D
#    rm ./CG/*.o ./FT/*.o ./LU/*.o ./IS/*.o ./common/*.o
#    make cg NPROCS=$i CLASS=D llvm=1
#    make ft NPROCS=$i CLASS=D llvm=1
#    make lu NPROCS=$i CLASS=D llvm=1
#    make is NPROCS=$i CLASS=D llvm=1
#    rm ./CG/*.o ./FT/*.o ./LU/*.o ./IS/*.o ./common/*.o
#done
#mv ./bin/cg.D.*.bc ./bin/ft.D.*.bc ./bin/lu.D.*.bc ./bin/is.D.*.bc ./bin/originbc
#mv ./bin/sp.D.* ./bin/bt.D.* ./bin/lu.D.* ./bin/is.D.* ./bin/originexec
#
#for(( i=2; i<=250; i++ ))
#do
#    make ep NPROCS=$i CLASS=D
#    rm ./EP/*.o ./common/*.o
#    make ep NPROCS=$i CLASS=D llvm=1
#    rm ./EP/*.o ./common/*.o
#done
#mv ./bin/ep.D.*.bc ./bin/originbc
#mv ./bin/ep.D.* ./bin/originexec

#dir=$(ls ./bin/bbtimebc/)
#
#for i in $dir
#do
#    echo $i
#    var=${i//./ }
#    arr=($var)
#    output=${arr[0]}"."${arr[1]}"."${arr[2]}
#    cd ./bin
#    opt -load ~/Document/projects/llvm-pred/build/src/libLLVMPred.so -insert-edge-profiling -insert-mpi-profiling ./originbc/$i -o ./edgebc/${output}".edge.bc"
#    opt -load /home/wzzhang/scratch/lgz/llvm-curvefit/build/src/libMemoryModel.so -BBTime ./originbc/$i -o ./bbtimebc/${output}".bbtime.bc"
#    llc -filetype=obj ./edgebc/${output}".edge.bc" -o ${output}".edge.o"
#    mpif90 ${output}".edge.o" -L/usr/local/lib `pkg-config llvm-prof --variable=profile_rt_lib` -o ./edgeexec/${output}".edge"
#    llc -filetype=obj -code-model=medium ./bbtimebc/${output}".bbtime.bc" -o ${output}".bbtime.o"
#    g++ -L/usr/local/lib -L/usr/local/lib/openmpi/ -L/home/wzzhang/usr-lmpi_mpifh -L/home/wzzhang/usr/lib/gcc/x86_64-redhat-linux7E/4.8.2/libgfortran.a -mcmodel=medium -ldl -lmpi_mpifh -lgfortran ${output}".bbtime.o" ../getbbtime.cpp -o ./bbtime_count_exec/${output}".bbtime"
#    rm *.o
#    cd ..
#done
