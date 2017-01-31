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
    //If the thing being dealt with is a negative number
    if(function.at(0) == '-')
        return new CalcTree(CalcTree::neg, generateTree(function.substr(1, function.length() - 1)));

    if(function == std::string("x"))
        return new CalcTree(CalcTree::x);

    if(isANum(function))
        return new CalcTree(stod(function));

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
//Switchable comparison function
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
            //Conditional to not conflate subtraction with minus signs
            if(ch != '-' || (function.at(c - 1) == ')' || isANum(function.at(c - 1))))
                return new CalcTree(generateTree(function.substr(0, c)), op,
                                    generateTree(function.substr(c + 1, function.length() - c - 1)));
        }
    }
    return NULL;
}

//Checks if char inchar is a number using the ASCII table
bool CalcMachine::isANum(std::string instr){
    for(unsigned a = 1; a < instr.size(); a++)
        if(!isANum(instr.at(a)))
            return false;
    return true;
}

bool CalcMachine::isANum(char inchar) {
    if((inchar < '0' || inchar > '9') && inchar != 46 && inchar != 120) return false;
    else return true;
}

/*
 * EVALUATION METHODS
 */
double recurseEvaluate(CalcTree* parsetree, double index = 0) {
    if(parsetree->op == CalcTree::null || parsetree->op == CalcTree::x) {
        if(parsetree->op == CalcTree::x) return index;
        else return parsetree->val;
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
        else if(parsetree->op == CalcTree::neg) return -recurseEvaluate(parsetree->leftbranch, index);
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
    //Integration by trapezoidal approximation
    for(double a = low_bound; a < high_bound; a += .001){
        ret += ((recurseEvaluate(root, a) + recurseEvaluate(root, a + .001)) / 2) * .001;
    }
    return ret;
}

std::string CalcMachine::derivative() {
    std::cout << "Computing derivative\n";
    CalcTree* ddx = computeDerivative(root);
    std::cout << "Simplifying derivative\n";
    CalcTree* ddx_old = NULL;
    do {
        if(ddx_old)
            delete ddx_old;
        ddx_old = new CalcTree(ddx);
        ddx = simplify(ddx);
    } while(*ddx != *ddx_old);
    delete ddx_old;
    std::cout << "Deconstructing derivative tree\n";
    std::string deriv = deconstruct(ddx, 0);
    delete ddx;
    return deriv;
}
//Checks recursively if a calctree contains an x somewhere in its structure
bool containsX(CalcTree* fxn) {
    if(!fxn) return false;
    else return (fxn->op == CalcTree::x || containsX(fxn->leftbranch) || containsX(fxn->rightbranch));
}

CalcTree* CalcMachine::computeDerivative(CalcTree* fxn) {
    if(fxn->op == CalcTree::x)
        return new CalcTree(1.0);
    else if(fxn->op == CalcTree::null)
        return new CalcTree(0.0);
    else if(fxn->op == CalcTree::add)
        return new CalcTree(computeDerivative(fxn->leftbranch), CalcTree::add, computeDerivative(fxn->rightbranch));
    else if(fxn->op == CalcTree::sub)
        return new CalcTree(computeDerivative(fxn->leftbranch), CalcTree::sub, computeDerivative(fxn->rightbranch));
    else if(fxn->op == CalcTree::mult) //Product rule
        return new CalcTree(new CalcTree(new CalcTree(fxn->leftbranch), CalcTree::mult, computeDerivative(fxn->rightbranch)),
                            CalcTree::add,
                            new CalcTree(computeDerivative(fxn->leftbranch), CalcTree::mult, new CalcTree(fxn->rightbranch)));
    else if(fxn->op == CalcTree::div) //Quotient rule
        return new CalcTree(new CalcTree(new CalcTree(new CalcTree(fxn->leftbranch), CalcTree::mult, computeDerivative(fxn->rightbranch)),
                                         CalcTree::sub,
                                         new CalcTree(computeDerivative(fxn->leftbranch), CalcTree::mult, new CalcTree(fxn->rightbranch))),
                            CalcTree::div,
                            new CalcTree(new CalcTree(fxn->rightbranch), CalcTree::exp, new CalcTree(2.0)));
    else if(fxn->op == CalcTree::exp) {
        if(containsX(fxn->leftbranch)) {
            if(containsX(fxn->rightbranch)) throw 1;
            //Power rule & chain rule
            else return new CalcTree(new CalcTree(new CalcTree(fxn->rightbranch),
                                                  CalcTree::mult,
                                                  new CalcTree(new CalcTree(fxn->leftbranch),
                                                               CalcTree::exp,
                                                               new CalcTree(new CalcTree(fxn->rightbranch), CalcTree::sub, new CalcTree(1.0)))),
                                     CalcTree::mult,
                                     computeDerivative(fxn->leftbranch));
        } else { //Chain rule
            return new CalcTree(new CalcTree(new CalcTree(fxn),
                                             CalcTree::div,
                                             new CalcTree(CalcTree::ln, new CalcTree(fxn->leftbranch))),
                                CalcTree::mult,
                                computeDerivative(fxn->rightbranch));
        }
    } else if(fxn->op == CalcTree::sin) //Chain rule
        return new CalcTree(new CalcTree(CalcTree::cos, new CalcTree(fxn->leftbranch)),
                            CalcTree::mult,
                            computeDerivative(fxn->leftbranch));
    else if(fxn->op == CalcTree::cos) //Chain rule
        return new CalcTree(new CalcTree(CalcTree::neg, new CalcTree(CalcTree::cos, new CalcTree(fxn->leftbranch))),
                            CalcTree::mult,
                            computeDerivative(fxn->leftbranch));
    else if(fxn->op == CalcTree::tan) //Chain rule
        return new CalcTree(computeDerivative(fxn->leftbranch),
                            CalcTree::div,
                            new CalcTree(new CalcTree(CalcTree::cos, new CalcTree(fxn->leftbranch)), CalcTree::exp, new CalcTree(2.0)));
    else if(fxn->op == CalcTree::ln) //Chain rule
        return new CalcTree(computeDerivative(fxn->leftbranch), CalcTree::div, new CalcTree(fxn->leftbranch));
    else if(fxn->op == CalcTree::neg)
        return new CalcTree(CalcTree::neg, computeDerivative(fxn->leftbranch));
    else return NULL;
}
//Recursive algebraic simplification method-also, welcome... to the world of conditionals!
CalcTree* CalcMachine::simplify(CalcTree* tree) {
    if(!tree) return tree;
    if(tree->op == CalcTree::x) return tree; //Don't even bother checking anything else
    else if(tree->op == CalcTree::null) return tree; //Ditto
    CalcTree zero(0.0), one(1.0); //Used for comparison purposes
    if(tree->op == CalcTree::add) {
        if(*tree->leftbranch == zero) { //Addition of zero
            CalcTree* temp = tree;
            tree = tree->rightbranch;
            temp->rightbranch = NULL;
            delete temp;
        } else if(*tree->rightbranch == zero) { //Addition of zero: the reckoning
            CalcTree* temp = tree;
            tree = tree->leftbranch;
            temp->leftbranch = NULL;
            delete temp;
        } else if(tree->leftbranch->isANum() && tree->rightbranch->isANum()) { //If the tree is raw arithmetic
            CalcTree* temp = new CalcTree(recurseEvaluate(tree->leftbranch) + recurseEvaluate(tree->rightbranch));
            delete tree;
            tree = temp;
        }
    } else if(tree->op == CalcTree::sub) {
        if(*tree->leftbranch == zero) { //If the tree is zero minus something
            tree->op = CalcTree::neg;
            delete tree->leftbranch;
            tree->leftbranch = tree->rightbranch;
            tree->rightbranch = NULL;
        } else if(*tree->rightbranch == zero) { //If the tree is something minues zero
            CalcTree* temp = tree;
            tree = tree->leftbranch;
            temp->rightbranch = NULL;
            delete temp;
        } else if(tree->leftbranch->isANum() && tree->rightbranch->isANum()) { //If the tree is raw arithemtic
            CalcTree* temp = new CalcTree(recurseEvaluate(tree->leftbranch) - recurseEvaluate(tree->rightbranch));
            delete tree;
            tree = temp;
        } else if(*tree->leftbranch == *tree->rightbranch) { //If the tree is f(x) - f(x)
            delete tree;
            tree = new CalcTree(0.0);
        }
    } else if(tree->op == CalcTree::mult) {
        if(tree->leftbranch->op == CalcTree::neg && tree->rightbranch->op == CalcTree::neg) { //If two negative numbers are multiplied
            CalcTree *ltemp = tree->leftbranch, *rtemp = tree->rightbranch;
            tree->leftbranch = ltemp->leftbranch;
            tree->rightbranch = rtemp->leftbranch;
            ltemp->leftbranch = NULL;
            rtemp->leftbranch = NULL;
            delete ltemp;
            delete rtemp;
        } else if(tree->leftbranch->op == CalcTree::neg) { //If the left term is negative
            CalcTree* temp = tree->leftbranch;
            tree->leftbranch = temp->leftbranch;
            temp->leftbranch = tree;
            tree = temp;
        } else if(tree->rightbranch->op == CalcTree::neg) { //If the right term is negative
            CalcTree* temp = tree->rightbranch;
            tree->rightbranch = temp->leftbranch;
            temp->leftbranch = tree;
            tree = temp;
        }
        if(*tree->leftbranch == zero || *tree->rightbranch == zero) { //If the tree is something times zero
            delete tree;
            tree = new CalcTree(0.0);
        } else if(*tree->leftbranch == one) { //If the tree is something times one
            CalcTree* temp = tree;
            tree = tree->rightbranch;
            temp->rightbranch = NULL;
            delete temp;
        } else if(*tree->rightbranch == one) {
            CalcTree* temp = tree;
            tree = tree->leftbranch;
            temp->leftbranch = NULL;
            delete temp;
        } else if(tree->leftbranch->isANum() && tree->rightbranch->isANum()) { //If the tree is raw arithmetic
            CalcTree* temp = new CalcTree(recurseEvaluate(tree->leftbranch) * recurseEvaluate(tree->rightbranch));
            delete tree;
            tree = temp;
        } else if(*tree->leftbranch == *tree->rightbranch) { //Swap f(x) * f(x) for f(x)^2
            delete tree->rightbranch;
            tree->op = CalcTree::exp;
            tree->rightbranch = new CalcTree(2.0);
        }
    } else if(tree->op == CalcTree::div) {
        if(tree->leftbranch->op == CalcTree::neg && tree->rightbranch->op == CalcTree::neg) { //If two negative numbers are multiplied
            CalcTree *ltemp = tree->leftbranch, *rtemp = tree->rightbranch;
            tree->leftbranch = ltemp->leftbranch;
            tree->rightbranch = rtemp->leftbranch;
            ltemp->leftbranch = NULL;
            rtemp->rightbranch = NULL;
            delete ltemp;
            delete rtemp;
        } else if(tree->leftbranch->op == CalcTree::neg) { //If the left term is negative
            CalcTree* temp = tree->leftbranch;
            tree->leftbranch = temp->leftbranch;
            temp->leftbranch = tree;
            tree = temp;
        } else if(tree->rightbranch->op == CalcTree::neg) { //If the right term is negative
            CalcTree* temp = tree->rightbranch;
            tree->rightbranch = temp->leftbranch;
            temp->leftbranch = tree;
            tree = temp;
        }
        if(*tree->rightbranch == zero) {
            std::cerr << "ERROR: Division by zero found in function reduction";
            exit(1);
        } else if(*tree->leftbranch == zero) { //If the tree is zero divided by something
            delete tree;
            tree = new CalcTree(0.0);
        } else if(tree->leftbranch->isANum() && tree->rightbranch->isANum()) { //if the tree is arithmetic
            CalcTree* temp = new CalcTree(recurseEvaluate(tree->leftbranch) / recurseEvaluate(tree->rightbranch));
            delete tree;
            tree = temp;
        } else if(*tree->leftbranch == *tree->rightbranch) { //If the tree is something divided by itself
            delete tree;
            tree = new CalcTree(1.0);
        }
    } else if(tree->op == CalcTree::exp) {
        if(tree->rightbranch == zero && tree->leftbranch == zero) { //Zero to the power of itself
            std::cerr << "ERROR: Raised zero to the power of itself in function reduction";
            exit(1);
        } else if(*tree->rightbranch == zero) { //Raising something to the power zero
            delete tree;
            tree = new CalcTree(1.0);
        } else if(*tree->rightbranch == one) { //Raising something to the power one
            CalcTree* temp = tree->leftbranch;
            tree->leftbranch = NULL;
            delete tree;
            tree = temp;
        } else if(*tree->leftbranch == zero) { //Raising zero to any power
            delete tree;
            tree = new CalcTree(0.0);
        } else if(*tree->leftbranch == one) { //Raising one to any power
            delete tree;
            tree = new CalcTree(1.0);
        } else if(tree->leftbranch->isANum() && tree->rightbranch->isANum()) { //if the tree is arithmetic
            CalcTree* temp = new CalcTree(std::pow(recurseEvaluate(tree->leftbranch), recurseEvaluate(tree->rightbranch)));
            delete tree;
            tree = temp;
        }
    } else if(tree->op == CalcTree::sin && tree->leftbranch->isANum()) { //Sines that are arithmetic
        CalcTree* temp = new CalcTree(std::sin(recurseEvaluate(tree->leftbranch)));
        delete tree;
        tree = temp;
    } else if(tree->op == CalcTree::cos && tree->leftbranch->isANum()) { //Cosines that are arithmetic
        CalcTree* temp = new CalcTree(std::cos(recurseEvaluate(tree->leftbranch)));
        delete tree;
        tree = temp;
    } else if(tree->op == CalcTree::tan && tree->leftbranch->isANum()) { //Tangents that are arithmetic
        CalcTree* temp = new CalcTree(std::tan(recurseEvaluate(tree->leftbranch)));
        delete tree;
        tree = temp;
    } else if(tree->op == CalcTree::ln && tree->leftbranch->isANum()) { //Natural logs that are arithmetic
        CalcTree* temp = new CalcTree(std::log(recurseEvaluate(tree->leftbranch)));
        delete tree;
        tree = temp;
    }
    //Second recursive pass
    tree->leftbranch = simplify(tree->leftbranch);
    tree->rightbranch = simplify(tree->rightbranch);
    return tree;
}

//A smarter to_string function
std::string to_string(double val) {
    std::string out = std::to_string(val);
    while(out.at(out.size() - 1) == '0')
        out = out.substr(0, out.size() - 1);
    if(out.at(out.size() - 1) == '.')
        out = out.substr(0, out.size() - 1);
    return out;
}

//Recursive tree deconstruction method
//The level parameter is used to make sure order of operations is preserved
//by making sure parentheses are added where needed
std::string CalcMachine::deconstruct(CalcTree* tree, int level) {
    std::string tmp = "";
    if(tree->op == CalcTree::add) {
        tmp = deconstruct(tree->leftbranch, 0) + "+" + deconstruct(tree->rightbranch, 0);
        if(level > 0) tmp = "(" + tmp + ")";
    } else if(tree->op == CalcTree::sub) {
        tmp = deconstruct(tree->leftbranch, 0) + "-" + deconstruct(tree->rightbranch, 0);
        if(level > 0) tmp = "(" + tmp + ")";
    } else if(tree->op == CalcTree::mult) {
        tmp = deconstruct(tree->leftbranch, 1) + "*" + deconstruct(tree->rightbranch, 1);
        if(level > 1) tmp = "(" + tmp + ")";
    } else if(tree->op == CalcTree::div) {
        tmp = deconstruct(tree->leftbranch, 1) + "/" + deconstruct(tree->rightbranch, 1);
        if(level > 1) tmp = "(" + tmp + ")";
    } else if(tree->op == CalcTree::exp) {
        tmp = deconstruct(tree->leftbranch, 2) + "^" + deconstruct(tree->rightbranch, 2);
        if(level > 2) tmp = "(" + tmp + ")";
    } else if(tree->op == CalcTree::sin) {
        tmp = "sin(" + deconstruct(tree->leftbranch, 0) + ")";
    } else if(tree->op == CalcTree::cos) {
        tmp = "cos(" + deconstruct(tree->leftbranch, 0) + ")";
    } else if(tree->op == CalcTree::tan) {
        tmp = "tan(" + deconstruct(tree->leftbranch, 0) + ")";
    } else if(tree->op == CalcTree::ln) {
        tmp = "ln(" + deconstruct(tree->leftbranch, 0) + ")";
    } else if(tree->op == CalcTree::neg) {
        tmp = "-" + deconstruct(tree->leftbranch, 3);
    } else if(tree->op == CalcTree::x) {
        tmp = "x";
    } else if(tree->op == CalcTree::null) {
        tmp = to_string(tree->val);
    }
    return tmp;
}