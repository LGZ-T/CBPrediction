#/bin/bash -x


if [ ! -f "../build/src/origin.bc" ]; then
    echo "Please copy origin.bc to llvm-curvefit/build/src/"
    exit 0
fi


opt-3.5 -load ../build/src/libMemoryMedol.so -BBTime ../buid/src/origin.bc -o ../build/src/bbtime.bc

clang++-3.5 ../BasicBlockTime/getbbtime.cpp ../build/src/bbtime.bc -o ../build/src/bbtime

rm bbtotaltime
for ((i=50;i<201;i=i+10)) 
do
    ../build/src/bbtime 100 100 $i 1>/dev/null 2>../build/src/"bbtime."$i
    cat ../build/src/"bbtime."$i |cut -d ' ' -f 2 |awk 'BEGIN{total=0}{total=total+$1}END{print total}'>>../build/src/bbtotaltime
    rm *.yaml
done


