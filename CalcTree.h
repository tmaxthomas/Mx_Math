#include <string>

class CalcTree {
public:
	enum Operators {add, sub, mult, div, exp, sin, cos, tan, ln, neg, null};
    CalcTree(Operators inop, CalcTree* inlbr, CalcTree* inrbr = NULL) : val(""), op(inop), leftbranch(inlbr), rightbranch(inrbr) {}
    CalcTree(std::string inval) : val(inval), op(null), leftbranch(NULL), rightbranch(NULL){}
	std::string val;
	Operators op;
	CalcTree* leftbranch;
	CalcTree* rightbranch;
    ~CalcTree() { if(leftbranch) delete leftbranch; if(rightbranch) delete rightbranch; }
};

inline bool operator== (const CalcTree& n1, const CalcTree& n2) {
    return n1.op == n2.op && n1.val == n2.val && n1.leftbranch == n2.leftbranch && n1.rightbranch == n2.rightbranch;
}