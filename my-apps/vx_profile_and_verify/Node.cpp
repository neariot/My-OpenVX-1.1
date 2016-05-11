#include "Node.hpp"

using namespace OpenVX;

Node::Node(Graph &graph, vx_kernel_e kernel_e, enum Target target_e)
{
	mKernel = Kernel::getKernel(kernel_e, target_e);
	vx_kernel vxKernel = mKernel->getVxKernel(graph.getContext());
	m_node = vxCreateGenericNode(graph.getVxGraph(), vxKernel);
	GET_STATUS_CHECK(m_node);
}

Node::~Node()
{
	delete mKernel;
}

vx_node Node::getVxNode() const
{
	return m_node;
}

void Node::connect(int n, ...)
{
	va_list parameter_list;
	va_start(parameter_list, n);

	for (int i = 0; i < n; i++)
	{
		vx_reference parameter_i = va_arg(parameter_list, vx_reference);
		vx_status status = vxSetParameterByIndex(m_node, i, parameter_i);
		ERROR_CHECK(status);
	}

	va_end(parameter_list);
}
