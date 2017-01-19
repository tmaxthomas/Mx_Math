#include <string>

class CalcTree {
public:
	enum Operators {add, sub, mult, div, exp, sin, cos, tan, ln, neg, x, null};
    CalcTree(Operators inop, CalcTree* inlbr = NULL, CalcTree* inrbr = NULL) : val(0), op(inop), leftbranch(inlbr), rightbranch(inrbr) {}
    CalcTree(double inval) : val(inval), op(null), leftbranch(NULL), rightbranch(NULL) {}
    CalcTree(CalcTree* old) : val(old->val), op(old->op), leftbranch(old->leftbranch), rightbranch(old->rightbranch) {}
	double val;
	Operators op;
	CalcTree* leftbranch;
	CalcTree* rightbranch;
    ~CalcTree() { if(leftbranch) delete leftbranch; if(rightbranch) delete rightbranch; }
    bool isANum();
};

inline bool operator== (const CalcTree& n1, const CalcTree& n2) {
    return n1.op == n2.op && n1.val == n2.val && n1.leftbranch == n2.leftbranch && n1.rightbranch == n2.rightbranch;
}