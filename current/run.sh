#!/bin/bash

# Run instructions

echo "@@@ VIRGINIA SCRIPT @@@"
echo ""

cd program/

# Source address (ijk)
for i in `seq 0 1`;
do
	for j in `seq 0 1`;
	do
		for k in `seq 0 1`;
		do
			# Target address (xyz)
			for x in `seq 0 1`;
			do
				for y in `seq 0 1`;
				do
					for z in `seq 0 1`;
					do
						../virginia -t $i$j$k-$x$y$z -o ../outputs/$i$j$k-$x$y$z.out erl -run omega main 8 $i$j$k $x$y$z -run init stop -noshell
						echo ""
					done
				done
			done
		done
	done
done
