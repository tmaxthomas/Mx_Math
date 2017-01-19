//
// Created by Max on 1/19/2017.
//

#include "CalcTree.h"
//Determines if a calctree represents a number, be it positive or negative
bool CalcTree::isANum() {
    if(op == neg) return leftbranch->isANum();
    else return op == null;
}
