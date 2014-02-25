/*
 * Modified by Okamoto on 2011-03-25
 */

#include "TransformStack.h"
#include "Transform.h"
#include <assert.h>

TransformStack::TransformStack() : m_curr(NULL)
{
	m_curr = new Transform();
}


TransformStack::TransformStack(const Transform &t) : m_curr(NULL)
{
	m_curr = new Transform(t);
}


TransformStack::~TransformStack()
{
	while (!m_stack.empty()) {
		Transform *p = m_stack.top();
		m_stack.pop();
		delete p;
			
	}
	delete m_curr;
}
		
void	TransformStack::push()
{
	Transform *t = new Transform(*m_curr);
	m_stack.push(t);
}

void	TransformStack::pop()
{
	assert(!m_stack.empty());
	delete m_curr;
	m_curr = m_stack.top();
	m_stack.pop();
}
