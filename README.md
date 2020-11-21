# Trees + Visualisation
Implementation of unbalanced N-ary trees and ability to visualize them by using Graphviz

## Description
    Expression written in format ((constant1) op (constant2))
    (each node is enclosed in brackets) is transformed into tree,
    and tree is exported in format, readable by dot.
    Then picture of resulting graph is being created.

## Starting
    Run 'make tree', then you have executable file tree, 
    which requires input parameter: input file
    The result of the program are two files: .dot and .png;
## Debug
    To turn debug on run make command with 'DEBUG=YES'
    It turns on -g option

### Dependences
    Linux, g++, make, eog, dot

## Documentation
To see the whole documentation, download source code and run 'doxywizard Documentation/Config'
(may be you should also update in dozywizard 'directory in which run doxywizard')
