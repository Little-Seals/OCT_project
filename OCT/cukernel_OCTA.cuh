#ifndef _CuKernel_OCTA_H
#define _CuKernel_OCTA_H
#include "cuhelper.cuh"
#include "cufinufft.h"
#define NUM1024 1024
#define NUM2048 2048
extern "C" {
	//MB扫描模式下，复数去相关算法(CC算法)
	void MB2D_ComplexDeCorrAlgorithm(uint16_t* dev_rawApoData, uint16_t* hos_rawApoData, //ApoData
		uint16_t* dev_rawSpecData, uint16_t* hos_rawSpecData, //SpecData
		float* dev_apoData,  //ApoData in GPU，Preloaded On Initialization
		uint32_t AvgNsize, uint32_t ApoNsize,
		uint32_t ObjAsize, uint32_t ObjBsize,
		cufftComplex* dev_complexData, cufftComplex* dev_complexDataX, //midData
		cufinufftf_plan dplan, cudaStream_t stream, //cudaParam
		float* dev_outputData, float* hos_outputData); //outputData
	//BM扫描模式下，复数去相关算法(CC算法)



	//The BM3D Kernel Function Declaration



}
#endif