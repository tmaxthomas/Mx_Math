# Mx_Math
This is a translation, modification, extension, and improvement on an earlier project written in Java. I've been translating to C++, in the format of a terminal application, modifying some of the inner logic (most notably negative number handling), and extending the program's capabilities.

In brief, this program uses trees to parse, evaluate, and modify functions in order to perform several sorts of mathematical operations, such as computation of derivatives.

How to run this program:
The first program argument should be the function the user wants parsed. Refer to guidelines for function syntax for details about syntax
Instructions for the various second arguments are as follows:

evaluation: Evaluates the function at the given value of x (third argument)



Guidelines for function syntax:
Whitespace: Whitespace characters are to be avoided
Trigonometry and other operators:
Sine -> sin(*expression*)
Cosine -> cos(*expression*)
Tangent -> tan(*expression*)
Natural logarithm -> ln(*expression*)
Parentheses: Don't nest them. So, don't do this: ((x + 3)). This, however, is fine: ((x + 3) * 2)