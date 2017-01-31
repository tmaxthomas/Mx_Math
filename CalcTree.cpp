//
// Created by Max on 1/19/2017.
//

#include "CalcTree.h"

bool db_comp(double a, double b){
    return a - b < .0001;
}

bool operator== (const CalcTree& n1, const CalcTree& n2) {
    bool ret = true;
    ret &= n1.op == n2.op;
    ret &= db_comp(n1.val, n2.val);
    ret &= (!n1.leftbranch && !n2.leftbranch) || (n1.leftbranch && n2.leftbranch && *n1.leftbranch == *n2.leftbranch);
    ret &= (!n1.rightbranch && !n2.rightbranch) || (n1.rightbranch && n2.rightbranch && *n1.rightbranch == *n2.rightbranch);
    return ret;
}

bool operator!= (const CalcTree& n1, const CalcTree& n2) {
    return !(n1 == n2);
}

//Determines if a calctree represents a number, be it positive or negative
bool CalcTree::isANum() {
    if(op == neg) return leftbranch->isANum();
    else return op == null;
}

CalcTree::~CalcTree() {
    if(leftbranch)
        delete leftbranch;
    if(rightbranch)
        delete rightbranch;
}
