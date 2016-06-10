#include <iostream>
#include <vector>
#include "vx.hpp"
#include "CoarsenScheduling.h"
#include "ProfileData.h"
#define N_NODES 7+2
#define N_TIMES 100
#define IMG_WIDTH 640
#define IMG_HEIGHT 480
#define SRC_IMG_NAME1 "lena.jpg"
#define CSV_NAME "scheduling.csv"
#define NS_TO_MS (1000*1000*1.0)
using namespace cv;
using namespace OpenVX;

int main(int argc, char **argv)
{
	Context context;
	context.selfTest();
	logs() << std::endl;

	Graph graph(context);
	Node *nodes[N_NODES];

	Mat lena_src = imread(SRC_IMG_NAME1);
	NULLPTR_CHECK(lena_src.data);
	resize(lena_src, lena_src, Size(IMG_WIDTH, IMG_HEIGHT));
	cvtColor(lena_src, lena_src, CV_RGB2GRAY);

	Image in(context, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8, lena_src);
	VirtualImage tmp01(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	VirtualImage tmp123(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	VirtualImage tmp24(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	VirtualImage tmp35(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	VirtualImage tmp46(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	VirtualImage tmp56(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	Image out(context, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);

	VirtualImage tmp68(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	VirtualImage tmp78(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);

	vx_kernel_e kernel_es[N_NODES] = {
		VX_KERNEL_BOX_3x3, VX_KERNEL_GAUSSIAN_3x3,
		VX_KERNEL_NOT, VX_KERNEL_GAUSSIAN_3x3,
		VX_KERNEL_BOX_3x3, VX_KERNEL_NOT,
		VX_KERNEL_XOR,
		VX_KERNEL_GAUSSIAN_3x3, VX_KERNEL_AND
	};

	ProfileData profileData;
	Target targets[N_NODES];
	nodeCoarsen(profileData, N_NODES, kernel_es, targets);

	for (int i = 0; i < N_NODES; i++)
	{
		nodes[i] = graph.addNode(kernel_es[i], targets[i]);
	}

	nodes[0]->connect(2, in.getVxImage(), tmp01.getVxImage());
	nodes[1]->connect(2, tmp01.getVxImage(), tmp123.getVxImage());
	nodes[2]->connect(2, tmp123.getVxImage(), tmp24.getVxImage());
	nodes[3]->connect(2, tmp123.getVxImage(), tmp35.getVxImage());
	nodes[4]->connect(2, tmp24.getVxImage(), tmp46.getVxImage());
	nodes[5]->connect(2, tmp35.getVxImage(), tmp56.getVxImage());
	nodes[6]->connect(3, tmp46.getVxImage(), tmp56.getVxImage(), tmp68.getVxImage());

	nodes[7]->connect(2, tmp123.getVxImage(), tmp78.getVxImage());
	nodes[8]->connect(3, tmp68.getVxImage(), tmp78.getVxImage(), out.getVxImage());

	if (graph.verify())
	{
		Graph *mGraph = &graph;
		vx_kernel_e *mKernel_es = kernel_es;
		int n_nodes = N_NODES;
		int n_times = N_TIMES;
		fstream csv(CSV_NAME, std::fstream::out);

		vx_perf_t graphPref, firstTimePerfG;
		vx_perf_t *firstTimeComputeN = new vx_perf_t[n_nodes];
		vx_perf_t *firstTimePerfN = new vx_perf_t[n_nodes];

		mGraph->process();
		firstTimePerfG = mGraph->getPerformance();
		for (int n = 0; n<n_nodes; n++)
		{
			firstTimePerfN[n] = nodes[n]->getPerformance();
			firstTimeComputeN[n] = nodes[n]->getComputationTime();
		}

		for (int i = 0; i<n_times; i++)
			mGraph->process();

		logs().precision(2);
		logs() << std::fixed;

		graphPref = mGraph->getPerformance();
		logs() << "   "
			<< "first: " << firstTimePerfG.tmp / NS_TO_MS << " ms "
			<< "min: " << graphPref.min / NS_TO_MS << " ms "
			<< "max: " << graphPref.max / NS_TO_MS << " ms "
			<< "avg: " << (graphPref.sum - firstTimePerfG.sum) / (n_times*NS_TO_MS) << " ms " << std::endl;

		for (int n = 0; n<n_nodes; n++)
		{
			vx_perf_t nodePerf = nodes[n]->getPerformance();
			vx_perf_t nodeCompute = nodes[n]->getComputationTime();
			logs() << "\tnode[" << n << "] - " << std::endl
				<< "\t total    "
				<< "first: " << firstTimePerfN[n].tmp / NS_TO_MS << " ms "
				<< "min: " << nodePerf.min / NS_TO_MS << " ms "
				<< "max: " << nodePerf.max / NS_TO_MS << " ms "
				<< "avg: " << (nodePerf.sum - firstTimePerfN[n].sum) / (n_times*NS_TO_MS) << " ms " << std::endl
				<< "\t compute  "
				<< "first: " << firstTimeComputeN[n].tmp / NS_TO_MS << " ms "
				<< "min: " << nodeCompute.min / NS_TO_MS << " ms "
				<< "max: " << nodeCompute.max / NS_TO_MS << " ms "
				<< "avg: " << (nodeCompute.sum - firstTimeComputeN[n].sum) / (n_times*NS_TO_MS) << " ms " << std::endl
				<< "\t transfer "
				<< "first: " << (firstTimePerfN[n].tmp - firstTimeComputeN[n].tmp) / NS_TO_MS << " ms "
				<< "min: " << (nodePerf.min - nodeCompute.min) / NS_TO_MS << " ms "
				<< "max: " << (nodePerf.max - nodeCompute.min) / NS_TO_MS << " ms "
				<< "avg: " << ((nodePerf.sum - firstTimePerfN[n].sum) -
				(nodeCompute.sum - firstTimeComputeN[n].sum)) / (n_times*NS_TO_MS) << " ms "
				<< std::endl;

			csv << Kernel::getFullKernelName(mKernel_es[n], targets[n]) << ","
				<< (nodeCompute.sum - firstTimeComputeN[n].sum) / (n_times*NS_TO_MS) << ","
				<< ((nodePerf.sum - firstTimePerfN[n].sum) -
				(nodeCompute.sum - firstTimeComputeN[n].sum)) / (n_times*NS_TO_MS) << std::endl;
		}

		csv.close();
		delete[] firstTimePerfN;
		delete[] firstTimeComputeN;
	}

	for (int n = 0; n<N_NODES; n++)
		graph.removeNode(nodes[n]);

	logs() << argv[0] << " done!!" << endl;
	logs().close();
	return 0;
}