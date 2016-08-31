#/bin/bash 


for ((i=50;i<201;i=i+10)) 
do
    perf stat -e L1-dcache-loads,L1-dcache-load-misses,L1-dcache-stores,LLC-loads,LLC-load-misses,LLC-stores ../build/src/origin 100 100 $i 1>/dev/null 2>>cachehitperf
    rm *.yaml
done


