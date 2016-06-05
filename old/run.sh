#!/bin/bash

echo Executing sequential tests
#echo ""

for filename in test/*.in;
do
    echo -e "\t$filename"
    sequential/smooth $filename sequential >> stats/sequentialMetrics.mtr
done

echo ""
echo Executing parallel tests
#echo ""

for filename in test/*.in;
do
    echo -e "\t$filename"
    parallel/smoothparallel $filename parallel >> stats/parallelMetrics.mtr
done

#mv test/*.out parallel/

echo ""
echo Scanning for output differences...

for sequentialResult in $(find sequential/ -name '*.out');
do
    sfname=$(basename $sequentialResult)
    #echo $sfname

    for parallelResult in $(find parallel/ -name '*.out');
    do
        pfname=$(basename $parallelResult)
        #echo $pfname

        if [ "$sfname" = "$pfname" ];
        then
            diff $sequentialResult $parallelResult
        fi
    done
done

echo ""
