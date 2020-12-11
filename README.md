# Trees + Visualisation
Implementation of unbalanced N-ary trees and ability to visualize them by using Graphviz
Result:
![alt text](https://github.com/GetsuDer/TreesAndVisualisation/blob/main/formula.jpg)
![alt text](https://github.com/GetsuDer/TreesAndVisualisation/blob/main/graph.png)
## Description
### First part. Full_Parenthesis
    Expression written in format ((constant1) op (constant2))
    (each node is enclosed in brackets) is transformed into tree,
    and tree is exported in format, readable by dot.
    Then picture of resulting graph is being created and pdf with source formula.
    Now are available: source expression, simplified expression, 
    derivate for 'x' (derivations for all variables can be taken, of course)

### Second part. Recursive descent
    Expression written in normal format, for example, 1 + 3 * (4 + 5).
    Arguments for sin, cos, ln must be in brackets (sin (12), for example).
    To get program, run 'make rec_desc'.
    Then './rec_desc input_file show' will calculate expression (and if show == 1, 
            show it as picture)

## Starting
    Run 'make tree; cd Testing', then you have executable file tree, 
    which requires three input parameter: 
    input file, open_png_flag (0 or 1), open_pdf_flag (0 or 1)
    Example: "./../tree exp6.in 1 1" will open firstly .png, then .pdf
    The result of the program are four files: .dot, .tex, .pdf  and .png;

## Debug
    To turn debug on run make command with 'DEBUG=YES'
    It turns on -g option

## Testing
    Test result can be seen into Testing/Full_Pars/png and Testing/Rec_Desc/png etc.

### Full Parenthesis
    Tests are placed into 'Testing/Full_Pars'.
    Run 'make test'.
    Or from directory Testing run './../tree Full_Pars/testname.in show_png show_pdf'
    Run './../tree Full_Pars/testname.in 0 0" to get .png and .pdf files. 
    1 instead 0 opens corresponding files.
### Recursive Descent
    Tests are places into 'Testing/Rec_Desc'.
    Run 'make test_rec'.
    Or from directory Testing run './../rec_desc Rec_Desc/testname.in show'
    (show = 1, if you want to see the result immediately)

## Dependences
    Linux, g++, make, eog, dot, gio, pdftex

## Documentation
To see the whole documentation, download source code and run 'doxywizard Documentation/Config'
(may be you should also update in dozywizard 'directory in which run doxywizard')
