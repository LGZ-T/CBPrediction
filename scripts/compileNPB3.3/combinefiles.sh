#!/bin/bash

cd bbtime_count_exec
rootdir=$(ls .)
ty=""
size=""
procs=""
ite=""
bbid=""
for i in ${rootdir[@]}
do
    echo $PWD
    echo $i
    var=${i//./ }
    arr=($var)
    ty=${arr[0]}
    size=${arr[1]}
    procs=${arr[2]}
    ite=${arr[4]}
    childfiles=$(ls ./$i/)
    childarr=($childfiles)
    cd ./$i
#    pretotalcycles=0
    prelinecount=0
    maxindex=0
    index=0
    for j in ${childarr[@]}
    do
#        totalcycles=$(awk 'BEGIN {totalcycles=0}{totalcycles+=$2;} END {printf("%.0f\n",totalcycles);}' $j)
        curlinecount=$(awk 'END{print NR}' $j)
        if [ $curlinecount -gt $prelinecount ]; then
            prelinecount=$curlinecount
            maxindex=$index
        fi
        ((index++))
    done
    awk -v ty=$ty -v size=$size -v procs=$procs -v ite=$ite '{print ty"\t"size"\t"procs"\t"ite"\t"$1"\t"$2"\t"$3}' ${childarr[$maxindex]} >> ~/$ty
    cd ..
done
