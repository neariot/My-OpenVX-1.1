/*
 * Copyright (c) 2016 National Tsing Hua University.
 * \brief The OpenCL OpenVX Kernel Interfaces
 * \author Tzu-Hsiang Lin <thlin@pllab.cs.nthu.edu.tw>
 */

#ifndef _VX_HEXAGON_MODEL_H_
#define _VX_HEXAGON_MODEL_H_

#include <VX/vx.h>
#include <VX/vx_helper.h>
/* TODO: remove vx_compatibility.h after transition period */
#include <VX/vx_compatibility.h>

#ifdef __cplusplus
extern "C" {
#endif

vx_status vxHexagonInit();
vx_status vxHexagonDeInit();

vx_status vxNot(vx_node node, vx_image input, vx_image output);
vx_status vxBox3x3(vx_node node, vx_image src, vx_image dst, vx_border_t *bordermode);
vx_status vxGaussian3x3(vx_node node, vx_image src, vx_image dst, vx_border_t *bordermode);
vx_status vxMedian3x3(vx_node node, vx_image src, vx_image dst, vx_border_t *bordermode);
vx_status vxAnd(vx_node node, vx_image in0, vx_image in1, vx_image output);
vx_status vxXor(vx_node node, vx_image in0, vx_image in1, vx_image output);
vx_status vxAddition(vx_node node, vx_image in0, vx_image in1, vx_scalar policy_param, vx_image output);
vx_status vxSubtraction(vx_node node, vx_image in0, vx_image in1, vx_scalar policy_param, vx_image output);
vx_status vxThreshold(vx_node node, vx_image src_image, vx_threshold threshold, vx_image dst_image);

#ifdef __cplusplus
}
#endif

#endif