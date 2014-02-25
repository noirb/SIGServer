/*
 * Created by Okamoto on 2011-03-25
 */

#ifndef TransformStack_h
#define TransformStack_h

#include <stack>

class Transform;


class TransformStack
{
private:
	typedef std::stack<Transform*> Stack;
private:
	Stack    m_stack;
	Transform	*m_curr;
public:
	TransformStack();
	TransformStack(const Transform &t);
	~TransformStack();

	void	push();
	void	pop();
	
	Transform & curr() { return *m_curr; }
};

#endif // TransformStack_h
 

