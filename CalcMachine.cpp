//
// Created by Max on 12/26/2016.
//

#define E "2.718281828459"
#define PI "3.1415926535859"

#include "CalcMachine.h"

/*
 * There's a lot of passing strings by value in this code. Now, while it may be slow, it's not easily
 * avoidable, since a lot of substrings need to be thrown around. The alternative would be to pass
 * indices and substring lengths, but doing that disposes of all the nice features of the std::string
 * library-and using the passed indices and lengths to create substrings, in order to use std::string,
 * defeats the whole point.
 */

CalcMachine::CalcMachine(const std::string& fn) : function(fn){
    try {
        format();
        std::cout << "Generating tree\n";
        root = generateTree(function);
    } catch(int exception) {
        std::cerr << "ERROR: IMPROPERLY FORMATTED INPUT STRING: " << function << "\n";
        exit(1);
    }
}

/*
 * STRING FORMATTING METHODS
 */

//Method that formats the input string to a more easily parsed form
void CalcMachine::format() {
    if(function.at(0) == '"')
        function = function.substr(1, function.length() - 2);
    //Removes spaces
    for(unsigned a = 0; a < function.length(); a++) {
        if(function.at(a) == ' ') {
            function.erase(a);
            a--;
        }
    }

    //Makes implicit multiplication explicit(ex: 3(x+4) -> 3*(x+4) )
    for(unsigned a = 1; a < function.length(); a++) {
        if(function.at(a) == 'x' || function.at(a) == '(') {
            if((isANum(function.at(a - 1)) || function.at(a - 1) == 'x') || (function.at(a) == '(' && function.at(a - 1) == ')')) {
                function.insert(a, "*");
                a++;
            }
        }
    }

    //Converts substrings "e" and "pi" to the corresponding string representation of the _USE_MATH_DEFINES const double form of the corresponding irrational numbers
    for(unsigned a = 0; a < function.length(); a++) {
        if(function.at(a) == 'e') {
            function.erase(a);
            function.insert(a, E);
        } else if(function.substr(a, 2) == "pi") {
            function.erase(a);
            function.insert(a, PI);
        }
    }
}

/*
 * TREE GENERATION METHODS
 */

//Method to recursively generate tree
//String is passed by value because of need to pass substrings-copying of strings is unavoidable

CalcTree* CalcMachine::generateTree(std::string function) {
    unsigned closure = 0; //Variable used in code designed to skip over sections of the function enclosed in parentheses
    //This for loop makes sure the input string is not enclosed within superfluous parentheses, such as those in the string "(3+x)"
    for(unsigned a = 0; a < function.size(); a++) {
        if(function.at(a) == '(')
            closure++;
        else if(function.at(a) == ')')
            closure--;
        if(closure == 0 && a != function.length() - 1)
            break;
        else if(a != 0 && closure == 0 && a == function.length() - 1)
            function = function.substr(1, function.length() - 2);
    }

    if(isANum(function))
        return new CalcTree(function);

    CalcTree* ret_ptr = binaryOpGenerator(CalcTree::add, '+', true, function);
    if(ret_ptr != NULL) return ret_ptr;

    ret_ptr = binaryOpGenerator(CalcTree::sub, '-', false, function);
    if(ret_ptr != NULL) return ret_ptr;

    ret_ptr = binaryOpGenerator(CalcTree::mult, '*', true, function);
    if(ret_ptr != NULL) return ret_ptr;

    ret_ptr = binaryOpGenerator(CalcTree::div, '/', false, function);
    if(ret_ptr != NULL) return ret_ptr;

    ret_ptr = binaryOpGenerator(CalcTree::exp, '^', true, function);
    if(ret_ptr != NULL) return ret_ptr;

    std::string opstr = function.substr(0, 3);
    if(opstr == "sin") return new CalcTree(CalcTree::sin, generateTree(function.substr(3, function.length() - 3)));
    else if(opstr == "cos") return new CalcTree(CalcTree::cos, generateTree(function.substr(3, function.length() - 3)));
    else if(opstr == "tan") return new CalcTree(CalcTree::tan, generateTree(function.substr(3, function.length() - 3)));
    else if(opstr == "ln(") return new CalcTree(CalcTree::ln, generateTree(function.substr(3, function.length() - 3)));
    throw 1;
}
//Switchable incrementer/decrementer
void incFunc(int& inc, bool dir){
    if(dir) ++inc;
    else --inc;
}

bool comp(int a, int b, bool dir){
    if(dir) return a < b;
    else return a > b;
}

//Helper function for tree generation
CalcTree* CalcMachine::binaryOpGenerator(CalcTree::Operators op, char ch, bool dir, std::string& function){
    int closure = 0;
    int a, b;
    if(dir) { a = 0; b = function.size(); }
    else { a = function.size() - 1; b = 0; }
    for(int c = a; comp(c, b, dir); incFunc(c, dir)) {
        if(( dir && function.at(c) == '(') || (!dir && function.at(c) == ')')) {
            closure = 1;
            while(closure != 0) {
                incFunc(c, dir);
                if(( dir && function.at(c) == '(') || (!dir && function.at(c) == ')')) closure++;
                else if(( dir && function.at(c) == ')') || (!dir && function.at(c) == '(')) closure--;
            }
        }
        if(function.at(c) == ch) {
            if (ch != '-' || (isANum(function.at(c - 1)) || function.at(c - 1) ==
                                                            ')')) //Conditional to deal with not confusing minus signs and subtraction
                return new CalcTree(op, generateTree(function.substr(0, c)),
                                    generateTree(function.substr(c + 1, function.length() - c - 1)));
        }
    }
    return NULL;
}

//Checks if char inchar is a number using the ASCII table
bool CalcMachine::isANum(std::string instr){
    if(instr.size() < 3) //All strings that short are necessarily numbers
        return true;
    if(!(instr.at(0) == '-' || isANum(instr.at(0))))
        return false;
    for(unsigned a = 1; a < instr.size(); a++)
        if(!isANum(instr.at(a)))
            return false;
    return true;
}

bool CalcMachine::isANum(char inchar) {
    if((inchar < 48 || inchar > 57) && inchar != 46 && inchar != 120) return false;
    else return true;
}

/*
 * EVALUATION METHODS
 */
double recurseEvaluate(CalcTree* parsetree, double index) {
    if(parsetree->op == CalcTree::null) {
        if(parsetree->val == "x") return index;
        else if(parsetree->val == "-x") return -index;
        else return stod(parsetree -> val);
    } else {
        if(parsetree->op == CalcTree::add) return recurseEvaluate(parsetree->leftbranch, index) + recurseEvaluate(parsetree->rightbranch, index);
        else if(parsetree->op == CalcTree::sub) return recurseEvaluate(parsetree->leftbranch, index) - recurseEvaluate(parsetree->rightbranch, index);
        else if(parsetree->op == CalcTree::mult) return recurseEvaluate(parsetree->leftbranch, index) * recurseEvaluate(parsetree -> rightbranch, index);
        else if(parsetree->op == CalcTree::div) return recurseEvaluate(parsetree->leftbranch, index) / recurseEvaluate(parsetree->rightbranch, index);
        else if(parsetree->op == CalcTree::exp) return std::pow(recurseEvaluate(parsetree->leftbranch, index), recurseEvaluate(parsetree->rightbranch, index));
        else if(parsetree->op == CalcTree::sin) return std::sin(recurseEvaluate(parsetree->leftbranch, index));
        else if(parsetree->op == CalcTree::cos) return std::cos(recurseEvaluate(parsetree->leftbranch, index));
        else if(parsetree->op == CalcTree::tan) return std::tan(recurseEvaluate(parsetree->leftbranch, index));
        else if(parsetree->op == CalcTree::ln) return std::log(recurseEvaluate(parsetree->leftbranch, index));
        else return 0;
    }
}

const double CalcMachine::evaluate(const double index) const{
    std::cout << "Evaluating\n";
    return recurseEvaluate(root, index);
}

const double CalcMachine::integrate(const double low_bound, const double high_bound) const{
    std::cout << "Integrating\n";
    double ret = 0;
    for(double a = low_bound; a < high_bound; a += .001){
        ret += ((recurseEvaluate(root, a) + recurseEvaluate(root, a + .001)) / 2) * .001;
    }
    return ret;
}
//Recursive function to determine if a tree represents a function of x
const bool containsX(CalcTree* func){
    if(!func)
        return false;
    else
        return (func->op == CalcTree::null && (func->val == std::string("x") || func->val == std::string("-x"))) ||
               containsX(func->leftbranch) || containsX(func->rightbranch);
}

CalcTree* computeDerivative(CalcTree* func){
    if(func->op == CalcTree::add)
        return new CalcTree(CalcTree::add, computeDerivative(func->leftbranch), computeDerivative(func->rightbranch));
    else if(func->op == CalcTree::sub)
        return new CalcTree(CalcTree::sub, computeDerivative(func->leftbranch), computeDerivative(func->rightbranch));
    else if(func->op == CalcTree::mult)
        return new CalcTree(CalcTree::add, new CalcTree(CalcTree::mult, func->leftbranch, computeDerivative(func->rightbranch)),
                            new CalcTree(CalcTree::mult, computeDerivative(func->leftbranch), func->rightbranch));
    else if(func->op == CalcTree::div)
        return new CalcTree(CalcTree::div, new CalcTree(CalcTree::sub, new CalcTree(CalcTree::mult, func->leftbranch, computeDerivative(func->rightbranch)),
                            new CalcTree(CalcTree::mult, computeDerivative(func->leftbranch), func->rightbranch)), new CalcTree(CalcTree::exp,
                            func->rightbranch, new CalcTree("2")));
    else if(func->op == CalcTree::exp);
        if(containsX(func->leftbranch));
}

CalcMachine CalcMachine::differentiate() {
    CalcTree* diff = computeDerivative(root);
}