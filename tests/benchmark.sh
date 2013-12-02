#!/bin/bash

file="best_result.txt"

if [ ! -f $file ]
then
    best=10000000000000000
else
    read best < $file
    best=$(($best+0))
fi

sum=0

for f in test-*.in
do
    out="$((java -cp ../bin/ TSP benchmark 2>&1) < $f)"
    sum=$(($sum + $out))
done

diff=$(($sum - $best))
if [ "$best" -gt $sum ]
then
  echo $sum > $file
  echo "Congrats! Improved result, with a difference of $diff."
else
  echo "Worse or equal result, with a difference of $diff."  
fi
 
echo "The final result was $sum."
	