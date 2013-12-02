#!/bin/bash
out="$((java -cp ../bin/ TSP 2>&1) < $1.in)"
echo "$out" > /tmp/$1.out
cat $1.in /tmp/$1.out | ../plottsp/plottsp