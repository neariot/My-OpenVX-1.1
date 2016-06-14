#include "Experiment.h"
#include "MyNode.h"
#include "RandomNodes.h"
using namespace cv;
using namespace std;
using namespace OpenVX;
#define SRC_IMG_NAME1 "lena.jpg"
#define SRC_IMG_NAME2 "baboon.jpg"
#define IMG_WIDTH 640
#define IMG_HEIGHT 480

Experiment::Experiment(const char *n, Context &c) 
	: name(n), context(c)
{
}

RandomFaceDetection::RandomFaceDetection(Context &c)
	: Experiment("Face detection", c)
{
}

void RandomFaceDetection::prepareNodesAndDatas(Graph &graph, vector<vx_kernel_e> &kernel_es, vector<MyNode *> &nodes)
{
	Mat lena_src = imread(SRC_IMG_NAME1);
	NULLPTR_CHECK(lena_src.data);
	resize(lena_src, lena_src, Size(IMG_WIDTH, IMG_HEIGHT));
	cvtColor(lena_src, lena_src, CV_RGB2GRAY);

	in = new Image(context, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8, lena_src);
	tmp01 = new VirtualImage(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	tmp123 = new VirtualImage(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	tmp24 = new VirtualImage(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	tmp35 = new VirtualImage(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	tmp46 = new VirtualImage(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	tmp56 = new VirtualImage(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	out = new Image(context, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);

	kernel_es.push_back(VX_KERNEL_BOX_3x3);
	kernel_es.push_back(VX_KERNEL_GAUSSIAN_3x3);
	kernel_es.push_back(VX_KERNEL_NOT);
	kernel_es.push_back(VX_KERNEL_GAUSSIAN_3x3);
	kernel_es.push_back(VX_KERNEL_BOX_3x3);
	kernel_es.push_back(VX_KERNEL_NOT);
	kernel_es.push_back(VX_KERNEL_XOR);

	MyNode::generateNodes(7, nodes, kernel_es);

	nodes[0]->connect(1, 1, in->getVxImage(), tmp01->getVxImage());
	nodes[1]->connect(1, 1, tmp01->getVxImage(), tmp123->getVxImage());
	nodes[2]->connect(1, 1, tmp123->getVxImage(), tmp24->getVxImage());
	nodes[3]->connect(1, 1, tmp123->getVxImage(), tmp35->getVxImage());
	nodes[4]->connect(1, 1, tmp24->getVxImage(), tmp46->getVxImage());
	nodes[5]->connect(1, 1, tmp35->getVxImage(), tmp56->getVxImage());
	nodes[6]->connect(2, 1, tmp46->getVxImage(), tmp56->getVxImage(), out->getVxImage());
}

void RandomFaceDetection::releaseDatas()
{
	delete in;
	delete out;
	delete tmp01;
	delete tmp123;
	delete tmp24;
	delete tmp35;
	delete tmp46;
	delete tmp56;
}

SuperResolution::SuperResolution(Context &c)
	: Experiment("Super resolution", c)
{
}

void SuperResolution::prepareNodesAndDatas(Graph &graph, vector<vx_kernel_e> &kernel_es, vector<MyNode *> &nodes)
{
	Mat lena_src = imread(SRC_IMG_NAME1);
	NULLPTR_CHECK(lena_src.data);
	resize(lena_src, lena_src, Size(IMG_WIDTH, IMG_HEIGHT));
	cvtColor(lena_src, lena_src, CV_RGB2GRAY);

	Mat baboon_src = imread(SRC_IMG_NAME2);
	NULLPTR_CHECK(baboon_src.data);
	resize(baboon_src, baboon_src, Size(IMG_WIDTH, IMG_HEIGHT));
	cvtColor(baboon_src, baboon_src, CV_RGB2GRAY);

	src1 = new Image(context, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8, lena_src);
	src2 = new Image(context, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8, baboon_src);
	subOut = new VirtualImage(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	thL = new VirtualImage(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	thR = new VirtualImage(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	result = new Image(context, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	vx_enum policy = VX_CONVERT_POLICY_WRAP;
	spolicy = vxCreateScalar(context.getVxContext(), VX_TYPE_ENUM, &policy);
	vx_enum thresh_type = VX_THRESHOLD_TYPE_BINARY;
	vx_enum data_type = VX_TYPE_UINT8;
	mThreshold = vxCreateThreshold(context.getVxContext(), thresh_type, data_type);
	vx_int32 threshold_value = 128;
	vxSetThresholdAttribute(mThreshold, VX_THRESHOLD_THRESHOLD_VALUE, &threshold_value, sizeof(vx_int32));

	kernel_es.push_back(VX_KERNEL_SUBTRACT);
	kernel_es.push_back(VX_KERNEL_THRESHOLD);
	kernel_es.push_back(VX_KERNEL_THRESHOLD);
	kernel_es.push_back(VX_KERNEL_ADD);

	MyNode::generateNodes(4, nodes, kernel_es);

	nodes[0]->connect(3, 1, src1->getVxImage(), src2->getVxImage(), spolicy, subOut->getVxImage());
	nodes[1]->connect(2, 1, subOut->getVxImage(), mThreshold, thL->getVxImage());
	nodes[2]->connect(2, 1, subOut->getVxImage(), mThreshold, thR->getVxImage());
	nodes[3]->connect(3, 1, thL->getVxImage(), thR->getVxImage(), spolicy, result->getVxImage());
}

void SuperResolution::releaseDatas()
{
	delete src1;
	delete src2;
	delete result;
	delete subOut;
	delete thL;
	delete thR;
	vxReleaseScalar(&spolicy);
	vxReleaseThreshold(&mThreshold);
}

RandomCase1::RandomCase1(Context &c) : Experiment("Random graph 1", c)
{
}

void RandomCase1::prepareNodesAndDatas(Graph &graph, std::vector<vx_kernel_e> &kernel_es, std::vector<MyNode *> &nodes)
{
	Mat lena_src = imread(SRC_IMG_NAME1);
	NULLPTR_CHECK(lena_src.data);
	resize(lena_src, lena_src, Size(IMG_WIDTH, IMG_HEIGHT));
	cvtColor(lena_src, lena_src, CV_RGB2GRAY);

	Mat baboon_src = imread(SRC_IMG_NAME2);
	NULLPTR_CHECK(baboon_src.data);
	resize(baboon_src, baboon_src, Size(IMG_WIDTH, IMG_HEIGHT));
	cvtColor(baboon_src, baboon_src, CV_RGB2GRAY);

	Mat xor_lena_baboon(IMG_HEIGHT, IMG_WIDTH, CV_8UC1);
	bitwise_xor(lena_src, baboon_src, xor_lena_baboon);

	src1 = new Image(context, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8, lena_src);
	src2 = new Image(context, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8, baboon_src);
	src3 = new Image(context, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8, xor_lena_baboon);
	v023 = new VirtualImage(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	v1345 = new VirtualImage(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	v267 = new VirtualImage(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	v489 = new VirtualImage(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	v378 = new VirtualImage(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	v59 = new VirtualImage(graph, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	dst6 = new Image(context, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	dst7 = new Image(context, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	dst8 = new Image(context, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);
	dst9 = new Image(context, IMG_WIDTH, IMG_HEIGHT, VX_DF_IMAGE_U8);

	kernel_es.push_back(OneIOneONodes::random(context, src1->getVxImage(), v023->getVxImage(), nodes));
	kernel_es.push_back(TwoIOneONodes::random(context, src2->getVxImage(), src3->getVxImage(), v1345->getVxImage(), nodes));
	kernel_es.push_back(OneIOneONodes::random(context, v023->getVxImage(), v267->getVxImage(), nodes));
	kernel_es.push_back(TwoIOneONodes::random(context, v023->getVxImage(), v1345->getVxImage(), v378->getVxImage(), nodes));
	kernel_es.push_back(OneIOneONodes::random(context, v1345->getVxImage(), v489->getVxImage(), nodes));
	kernel_es.push_back(OneIOneONodes::random(context, v1345->getVxImage(), v59->getVxImage(), nodes));
	kernel_es.push_back(OneIOneONodes::random(context, v267->getVxImage(), dst6->getVxImage(), nodes));
	kernel_es.push_back(TwoIOneONodes::random(context, v267->getVxImage(), v378->getVxImage(), dst7->getVxImage(), nodes));
	kernel_es.push_back(TwoIOneONodes::random(context, v378->getVxImage(), v489->getVxImage(), dst8->getVxImage(), nodes));
	kernel_es.push_back(TwoIOneONodes::random(context, v489->getVxImage(), v59->getVxImage(), dst9->getVxImage(), nodes));
}

void RandomCase1::releaseDatas()
{
	delete src1;
	delete src2;
	delete src3;
	delete v023;
	delete v1345;
	delete v267;
	delete v489;
	delete v378;
	delete v59;
	delete dst6;
	delete dst7;
	delete dst8;
	delete dst9;
}
