#!/bin/bash
time java -cp ../bin/ -agentlib:hprof=cpu=times TSP < $1.in