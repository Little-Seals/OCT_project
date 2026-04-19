#ifndef _CUKERNEL_H_
#define _CUKERNEL_H_
#include "cukernel_OCTA.cuh"
//CudaProcess类
class CudaProcess {
public:
	CudaProcess(); //default构造
	CudaProcess(uint32_t, uint32_t, uint32_t, uint32_t, bool);
	void CudaLoadComData(float*& hos_chirpData);
	void RunOctPro(uint16_t* hos_inputData, float* hos_retData);
	void RunOctaMB2D(uint16_t* hos_RawApoData, uint16_t* hos_RawSpecData, float* hos_retfloatData);
	~CudaProcess(); //析构函数
	cudaEvent_t start, stop;     //Cuda Event
private:
	bool ScanMode;
	uint32_t AvgNsize, ApoNsize; //均值&切趾
	uint32_t ObjAsize, ObjBsize; //目标尺寸
	uint32_t RawApoAsize, RawApoBsize; //切趾尺寸
	uint32_t RawSpecAsize, RawSpecBsize; //光谱尺寸
	//GPU数据指针
	cufinufft_opts opts;
	cufinufftf_plan dplan;
	cudaStream_t stream = 0;
	float* dev_apoData = nullptr; //DC文件
	float* dev_chirpData = nullptr; //插值文件
	uint16_t* dev_rawApoData = nullptr; //Apo(uint16_t)
	uint16_t* dev_rawSpecData = nullptr; //Spec(uint16_t)
	cufftComplex* dev_complexData = nullptr; //Raw(float32）
	cufftComplex* dev_complexDataX = nullptr; //Raw(float32)(mid data)
	float* dev_retfloatData = nullptr;
	//私有成员函数
	void CudaMemMalloc();
	void CudaMemDelete();
};
void nufftAssert(int code, const char* file, int line);
#define nufftErrchk(ans) { nufftAssert((ans), __FILE__, __LINE__); }
#endif