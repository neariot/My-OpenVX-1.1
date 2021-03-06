#include <iostream>
#include <vector>
#include "vx.hpp"
#include "Application.hpp"
#define N_TIMES 100
using namespace cv;
using namespace OpenVX;

void printNodesNames(Application *app, int variant)
{
	std::vector<std::string> nodesNames = app->getNodesName(variant);
	logs() << std::setw(2) << variant + 1 << "." << "node[0]: " << nodesNames[0] << std::endl;
	for (int i = 1; i < nodesNames.size(); i++)
	{
		logs() << "   " << "node[" << i << "]: " << nodesNames[i] << std::endl;
	}
}

int main(int argc, char **argv)
{
	Context context;
	context.selfTest();
	logs() << std::endl;

	std::vector<Application *> apps;
	AppOneIOneO::generateApps(context, &apps);
	AppTwoIOneO::generateApps(context, &apps);
	AppAddSub::generateApps(context, &apps);
	AppThreshold::generateApps(context, &apps);
	//App2Node_1I1O_1I1O::generateApps(context, &apps);
	//App2Node_2I1O_1I1O::generateApps(context, &apps);
	
	//apps.push_back(new AppTwoIOneO(context, VX_KERNEL_OR));		//NO fcv function
	//apps.push_back(new AppTableLookup(context));	//Not passed
	//apps.push_back(new AppHistogram(context));	//Not passed
	
	int n_apps = apps.size();
	
	logs() << "Process and Verify:" << std::endl;
	for (int i = 0; i < n_apps; i++)
	{
		logs() << "apps[" << i << "]: " << apps[i]->getKernelesType() << std::endl;

		apps[i]->prepareInput();
		int variants = apps[i]->getVariantCount();
		for (int v = 0; v < variants; v++)
		{
			printNodesNames(apps[i], v);
			
			apps[i]->setup();
			
			apps[i]->process(v);
			
			if (!apps[i]->verify())
				logs() << "\tverify fail" << std::endl;
			else
				logs() << "\tverify success" << std::endl;
			
			apps[i]->release();
		}
		apps[i]->releaseInput();
	}
	logs() << std::endl;
	
	logs() << "Profile " << n_apps << " apps over " << N_TIMES << " loop:" << std::endl;
	for (int i = 0; i < n_apps; i++)
	{
		logs() << "apps[" << i << "]: " << apps[i]->getKernelesType() << std::endl;

		apps[i]->prepareInput();
		int variants = apps[i]->getVariantCount();
		for (int v = 0; v < variants; v++)
		{
			printNodesNames(apps[i], v);
			
			apps[i]->setup();
			
			apps[i]->profiling(N_TIMES, v);
			
			apps[i]->release();
		}
		apps[i]->releaseInput();
	}
	logs() << std::endl;

	for (int i = 0; i < n_apps; i++)
		delete apps[i];
	
	logs() << argv[0] << " done!!" << endl;
	logs().close();
	//system("PAUSE");
	return 0;
}
