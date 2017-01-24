#include <string>

class CalcTree {
public:
	enum Operators {add, sub, mult, div, exp, sin, cos, tan, ln, neg, x, null};
    CalcTree(CalcTree* inlbr, Operators inop, CalcTree* inrbr) : val(0), op(inop), leftbranch(inlbr), rightbranch(inrbr) {}
    CalcTree(Operators inop, CalcTree* inlbr) : val(0), op(inop), leftbranch(inlbr), rightbranch(NULL) {}
    CalcTree(Operators inop) : val(0), op(inop), leftbranch(NULL), rightbranch(NULL) {}
    CalcTree(double inval) : val(inval), op(null), leftbranch(NULL), rightbranch(NULL) {}
    CalcTree(CalcTree* old) : val(old->val), op(old->op), leftbranch(old->leftbranch), rightbranch(old->rightbranch) {}
	double val;
	Operators op;
	CalcTree* leftbranch;
	CalcTree* rightbranch;
    ~CalcTree() { if(leftbranch) delete leftbranch; if(rightbranch) delete rightbranch; }
    bool isANum();
    friend bool operator== (const CalcTree& n1, const CalcTree& n2);
};