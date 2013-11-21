plottsp
=======

Shows an interactive plot of a Travelling Salesperson 2D Kattis
problem + solution (oldkattis:tsp)

Requirements
------------
Qt 4.x or 5.x.

Building
--------
Just type

    $ qmake
    $ make

to build the tool.

Documentation
-------------
The program reads a problem in the same format as described in
`oldkattis:tsp` from standard input, optionally followed by a
sequence of points specifying a solution (TSP tour), and presents
an interactive view. If `-p` is given, edges are specified as
point pairs instead (useful if you want to plot arbitrary
disconnected edges instead of a path).

### General Usage
    Usage: plottsp [ -h | -p ]
       -h     Show help message
       -p     Expect edges as point pairs (default is sequence)
    Input is read from standard input

### Available Controls
    Mouse Click+Drag     - Pan the view
    Ctrl + Scrollwheel   - Zoom

    Right                - Show next step of solution
    Ctrl + Right         - Show next 10 steps of solution
    Shift + Right        - Show next 100 steps of solution

    Left                 - Go back one step in the solution
    Ctrl + Left          - Go back 10 steps in the solution
    Shift + Left         - Go back 100 steps in the solution

    Ctrl + S             - Save current view to SVG
    Escape               - Quit

Example
-------
Assuming `test_problem.in` contains a problem and `test_solution.out`
a solution to this problem, then

    $ cat test_problem.in test_solution.out | ./plottsp

could result in something like (after punching `Right` a few times):

![screenshot][plottsp]

[plottsp]: https://raw.github.com/estan/plottsp/master/plottsp.png "Example view from plottsp"
