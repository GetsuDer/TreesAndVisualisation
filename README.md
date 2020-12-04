# Trees + Visualisation
Implementation of unbalanced N-ary trees and ability to visualize them by using Graphviz
Result:
![alt text](https://github.com/GetsuDer/TreesAndVisualisation/blob/main/formula.jpg)
![alt text](https://github.com/GetsuDer/TreesAndVisualisation/blob/main/graph.png)
## Description
    Expression written in format ((constant1) op (constant2))
    (each node is enclosed in brackets) is transformed into tree,
    and tree is exported in format, readable by dot.
    Then picture of resulting graph is being created and pdf with source formula.

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
    Run 'make test'.
    Or from directory Testing run './../tree testname.in show_png show_pdf'
    Run './../tree testname.in 0 0" to get .png and .pdf files. 
    1 instead 0 opens corresponding files.

### Dependences
    Linux, g++, make, eog, dot, gio, pdftex

## Documentation
To see the whole documentation, download source code and run 'doxywizard Documentation/Config'
(may be you should also update in dozywizard 'directory in which run doxywizard')
