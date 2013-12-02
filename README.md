tsp
===

To perform benchmarks:

    cd tests
    ./benchmark.sh

To generate a graph:

    cd graph
    ./mkgraph [ -h | -hn | -hu ] [ [ -n <spec> | -u <spec> ] ... ]
    
Generate a graph and test our program with it:

    cd tests
    ./graph/mkgraph -u 50,0,0,2000,2000 > test-6.in
    ./plot.sh test-6
    
To run a previous generated graph (in this case test-1.in):

    cd tests
    ./plot.sh test-1        