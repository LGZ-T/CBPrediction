#/bin/bash 


if [ ! -f "../build/src/l1tca" ]; then
    echo "Please copy cache programs to llvm-curvefit/build/src/"
    exit 0
fi

rm cachehit
for ((i=50;i<201;i=i+10)) 
do
    ../build/src/l1tca 100 100 $i 1>/dev/null 2>>../build/src/cachehit
    ../build/src/l1dch 100 100 $i 1>/dev/null 2>>../build/src/cachehit
    ../build/src/l2tch 100 100 $i 1>/dev/null 2>>../build/src/cachehit
    rm *.yaml
done


