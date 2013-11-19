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
The program reads a problem, optionally followed by a solution,
in the same format(s) as described in `oldkattis:tsp` and presents
an interactive view.

### Available Controls
    Mouse Click+Drag     - Pan the view
    Ctrl + Scrollwheel   - Zoom

    Right                - Show next step of solution
    Ctrl + Right         - Show next 10 steps of solution
    Shift + Right        - Show next 100 steps of solution

    Left                 - Go back one step in the solution
    Ctrl + Left          - Go back 10 steps in the solution
    Shift + Left         - Go back 100 steps in the solution

Example
-------
Assuming `test_problem.in` contains a problem and `test_solution.out`
a solution to this problem, then

    $ { cat test_problem.in && cat test_solution.out; } | ./plottsp

could result in something like

![screenshot][plottsp]

[plottsp]: https://github.com/estan/plottsp/plottsp.png "Example view from plottsp"
