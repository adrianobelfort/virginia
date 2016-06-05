#!/bin/bash

let i=0

widths=(512 640)
heights=(384 480)
frames=(500 1250 10 100 2 4000)

#widths=(10)
#heights=(10)
#frames=(2)

for n in "${frames[@]}"
do
    for w in "${widths[@]}"
    do
        for h in "${heights[@]}"
        do
            echo Generating movie$i.in with $n $w x $h frames
            ./generator movie$i $w $h $n
            let i=i+1
        done
    done
done
