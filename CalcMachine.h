//
// Created by Max on 12/26/2016.
//

#ifndef DERIVATIVE_TREE_CALCMACHINE_H
#define DERIVATIVE_TREE_CALCMACHINE_H

#include <string>
#include <iostream>
#include <cmath>
#include "CalcTree.h"

class CalcMachine{
public:
    CalcMachine(const std::string& function);
    const std::string& getFunction() const { return function; }
    const double evaluate(const double index) const;
    const double integrate(const double low_bound, const double high_bound) const;
    std::string derivative();
    ~CalcMachine() { delete root; }
private:
    //Internal methods
    CalcTree* generateTree(std::string function);
    CalcTree* binaryOpGenerator(CalcTree::Operators op, char ch, bool dir, std::string& function);
    void format();
    bool isANum(std::string instr);
    bool isANum(char inchar);
    CalcTree* computeDerivative(CalcTree* fxn);
    void simplify(CalcTree*& tree);
    //Member variables
    CalcTree* root;
    std::string function;
};

#endif //DERIVATIVE_TREE_CALCMACHINE_H
