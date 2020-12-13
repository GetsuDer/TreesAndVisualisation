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

### Second part. Programming language parsing
#### Program Structure
    Program must be a set of functions, Function is declared as 
    'function fun_name(param1_name, param2_name,...) {
        function_body;
    }'
    Params number may be zero. (function f() { ... }).

    The next operators are allowed:
##### If
    if (condition) { ...} else { ... }
    or
    if (condition) { ... }
##### While
    while (condition) { ... }
##### Return
    return ( ... );
##### For
    for (do_before; check_each_iteration; do_after_iteration) { ... }

##### Function call
    func_name(first_param, second_param, ...);

##### NB!
    return expression MUST be in brackets;
    Non-if, while or for statement in any place must be ended by ';';

#### How to see a parsed program?   
    To get the result program, run 'make rec_desc'.
    Then './rec_desc input_file show' will calculate expression (and if show == 1, 
            show it as picture)
#### Program example
    '
    function fib(n) {
        if (n < 2) {
            return (1);
        }
        return (n * fib(n - 1));
    }

    function main() {
        n = 5;
        res = f(n);
        print(res);
        return (0);
    }
    '
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
