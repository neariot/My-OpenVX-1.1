#include "Context.hpp"

using namespace OpenVX;

Context::Context()
{
	m_context = vxCreateContext();
	GET_STATUS_CHECK(m_context);
}

Context::~Context()
{
	vx_status status = vxReleaseContext(&m_context);
	ERROR_CHECK(status);
}

vx_status Context::setImmediateBorderMode(vx_border_t *config)
{
	return vxSetContextAttribute(getVxContext(), VX_CONTEXT_IMMEDIATE_BORDER, config, sizeof(*config));
}

vx_status Context::hint(vx_reference reference, vx_enum hint, const void* data, vx_size data_size)
{
	return vxHint(reference, hint, data, data_size);
}

vx_status Context::directive(vx_reference reference, vx_enum directive)
{
	return vxDirective(reference, directive);
}

vx_status Context::loadKernels(const char *filepath)
{
	return vxLoadKernels(getVxContext(), (vx_char *)filepath);
}

// vxQueryContext
void Context::selfTest()
{
	std::cout << "vxQueryContext:" << std::endl;
	std::cout << "\tvenderID: " << vendorID() << std::endl;
	std::cout << "\tversion: " << version().c_str() << std::endl;
	std::cout << "\tnumKernels: " << numKernels() << std::endl;
	std::cout << "\tnumModules: " << numModules() << std::endl;
	std::cout << "\tnumRefs: " << numRefs() << std::endl;
	std::cout << "\tnumTargets: " << numTargets() << std::endl;
	std::cout << "\timplementation: " << implementation() << std::endl;
	std::cout << "\textensions: " << extensions().c_str() << std::endl;
	std::cout << "\tconvolutionMaxDimension: " << convolutionMaxDimension() << std::endl;
}

vx_uint16 Context::vendorID()
{
	vx_uint16 vendor_id;
	vxQueryContext(getVxContext(), VX_CONTEXT_VENDOR_ID, &vendor_id, sizeof(vendor_id));
	return vendor_id;
}

std::string Context::version()
{
	vx_uint16 version;
	std::string version_name;
	vxQueryContext(getVxContext(), VX_CONTEXT_VERSION, &version, sizeof(version));
	switch (version)
	{
		case VX_VERSION_1_0:
			version_name.assign("VX_VERSION_1_0");
			break;
		case VX_VERSION_1_1:
			version_name.assign("VX_VERSION_1_1");
			break;
		default:
			version_name.assign("UNKNOWN");
			break;
	}
	return version_name;
}

vx_uint32 Context::numKernels()
{
	vx_uint32 kernels = 0;
	vxQueryContext(getVxContext(), VX_CONTEXT_UNIQUE_KERNELS, &kernels, sizeof(kernels));
	return kernels;
}

vx_uint32 Context::numModules()
{
	vx_uint32 modules = 0;
	vxQueryContext(getVxContext(), VX_CONTEXT_MODULES, &modules, sizeof(modules));
	return modules;
}

vx_uint32 Context::numRefs()
{
	vx_uint32 refs = 0;
	vxQueryContext(getVxContext(), VX_CONTEXT_REFERENCES, &refs, sizeof(refs));
	return refs;
}

#if defined(EXPERIMENTAL_USE_TARGET)
vx_uint32 Context::numTargets()
{
	vx_uint32 targets = 0;
	vxQueryContext(getVxContext(), VX_CONTEXT_TARGETS, &targets, sizeof(targets));
	return targets;
}
#endif

const char* Context::implementation()
{
	vxQueryContext(getVxContext(), VX_CONTEXT_IMPLEMENTATION, m_implementationName, sizeof(m_implementationName));
	return m_implementationName;
}

vx_size Context::extensionSize()
{
	vx_size size = 0;
	vxQueryContext(getVxContext(), VX_CONTEXT_EXTENSIONS_SIZE, &size, sizeof(size));
	return size;
}

std::string Context::extensions()
{
	std::string extensions_str;
	vx_size extsize = extensionSize();
	vx_char *p_extensionsStr = new vx_char[extsize];
	if (p_extensionsStr)
	{
		vxQueryContext(getVxContext(), VX_CONTEXT_EXTENSIONS, p_extensionsStr, extsize);
		extensions_str.assign(p_extensionsStr);
		delete [] p_extensionsStr;
	}
	return extensions_str;
}

vx_size Context::convolutionMaxDimension()
{
	vx_size size = 0;
	vxQueryContext(getVxContext(), VX_CONTEXT_CONVOLUTION_MAX_DIMENSION, &size, sizeof(size));
	return size;
}
