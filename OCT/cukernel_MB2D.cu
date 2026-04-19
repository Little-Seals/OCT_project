#include "cukernel_OCTA.cuh"
extern "C" { //MB2D CC Algorithm
	//=>dim3 blocks1(1, 1, 8), threads1(256, 1, 1);  256*8= 2048
	__global__ void MB2D_GetApoMean(float* ApoData, uint16_t* RawApoData, uint32_t ApoNsizeXAvgNsize) {
		uint32_t index = threadIdx.x + blockIdx.z * blockDim.x;
		float sum = 0.0f;
		for (uint32_t i = 0; i < ApoNsizeXAvgNsize; i++) {
			sum += RawApoData[index + i * NUM2048];
		}
		ApoData[index] = sum / ApoNsizeXAvgNsize;
	}
	//=>dim3 blocks1(AvgNsize, 1, ObjBsize), threads1(RawAsize/32, 1, 1);
	__global__ void MB2D_ApoAndHann(cufftComplex* complexData, uint16_t* RawSpecData, float* ApoData, uint32_t RawSpecAsize, uint32_t ObjBsize, uint32_t AvgNsize)
	{
		int idx = threadIdx.x; //沿着aline方向
		int index = idx + blockIdx.x * RawSpecAsize + blockIdx.z * RawSpecAsize * AvgNsize; //沿着aline、avgaline、bline方向
		while (idx < RawSpecAsize && index < RawSpecAsize * AvgNsize * ObjBsize)
		{
			float hannData = 0.5 - 0.5 * cos(idx * 6.2831853 / (RawSpecAsize - 1.00));
			complexData[index].x = hannData * (RawSpecData[index] - ApoData[idx]);
			complexData[index].y = 0;
			idx += blockDim.x;   //+=(ascanSize/@) < ascanSize
			index += blockDim.x; //+=(ascanSize/@) < ascanSize * bscanSize * blockSize
		}
	}
	//=>dim3 blocks(1, 1, ObjBsize), threads(RawAsize / 32 = 64, 1, 1);
	__global__ void MB2D_OctXYZtoXZY(cufftComplex* inputData, cufftComplex* outputData, uint32_t RawAsize, uint32_t AvgNsize, uint32_t ObjBsize)
	{
		//(RawAsize,AvgNsize,RawBsize) => (RawAsize,RawBsize,AvgNsize)
		int idx = threadIdx.x; //沿着aline方向
		int index1 = idx + blockIdx.z * RawAsize * AvgNsize; //沿着aline、avgaline、bline方向
		int index2 = idx + blockIdx.z * RawAsize;
		while (idx < RawAsize && index1 < RawAsize * AvgNsize * ObjBsize)
		{
			for (int i = 0; i < AvgNsize; i++) {
				outputData[index2 + i * (RawAsize * ObjBsize)] = inputData[index1 + i * RawAsize];
			}
			idx += blockDim.x;    //+=(ascanSize/@) < ascanSize
			index1 += blockDim.x; //+=(ascanSize/@) < ascanSize * bscanSize * blockSize
			index2 += blockDim.x; //+=(ascanSize/@) < ascanSize * bscanSize * blockSize
		}
	}
	//=>dim3 blocks(1, 1, ObjBsize), threads(RawAsize / 32 = 64, 1, 1);
	__global__ void MB2D_OctaProProcess(cufftComplex* complexData, uint32_t RawAsize, uint32_t ObjBsize, uint32_t AvgNsize)
	{
		int idx = threadIdx.x; //沿着aline方向
		int index = idx + blockIdx.z * RawAsize; //沿着aline、bline方向
		while (idx < (RawAsize / 2) && index < RawAsize * ObjBsize * AvgNsize)
		{
			uint32_t IdxAn = 0;
			for (int i = 0; i < AvgNsize - 1; i++) {
				IdxAn = index + NUM1024 + i * RawAsize * ObjBsize;
				//An.*Bn^
				complexData[IdxAn - NUM1024].x = complexData[IdxAn].x * complexData[IdxAn + RawAsize * ObjBsize].x + complexData[IdxAn].y * complexData[IdxAn + RawAsize * ObjBsize].y;
				complexData[IdxAn - NUM1024].y = complexData[IdxAn].y * complexData[IdxAn + RawAsize * ObjBsize].x - complexData[IdxAn].x * complexData[IdxAn + RawAsize * ObjBsize].y;
				//abs(An)
				complexData[IdxAn] = { complexData[IdxAn].x * complexData[IdxAn].x + complexData[IdxAn].y * complexData[IdxAn].y, 0.00f };
			}
			IdxAn = IdxAn + RawAsize * ObjBsize;
			complexData[IdxAn - NUM1024] = { 0.00f, 0.00f };
			complexData[IdxAn] = { complexData[IdxAn].x * complexData[IdxAn].x + complexData[IdxAn].y * complexData[IdxAn].y, 0.00f };
			idx += blockDim.x;   //+=(ascanSize/@) < ascanSize
			index += blockDim.x; //+=(ascanSize/@) < ascanSize * bscanSize * blockSize
		}
	}
	//=>dim3 blocks(1, 1, AvgNsize), threads(ObjBsize, 1, 1);
	__global__ void MB2D_SlidingWindows_x(cufftComplex* complexData, uint32_t RawAsize, uint32_t ObjBsize, uint32_t AvgNsize) {
		int i;
		int index = blockIdx.z * RawAsize * ObjBsize + threadIdx.x * RawAsize; //沿着aline、bline方向
		for (i = 0; i < NUM1024 - 1; i++) {
			complexData[index + i].x += complexData[index + i + 1].x;
			complexData[index + i].y += complexData[index + i + 1].y;
		}
		complexData[index + i].x += complexData[index + i].x;
		complexData[index + i].y += complexData[index + i].y;
		for (i = NUM1024; i < RawAsize - 1; i++) {
			complexData[index + i].x += complexData[index + i + 1].x;
		}
		complexData[index + i].x += complexData[index + i].x;
	}
	//=>dim3 blocks(1, 1, AvgNsize), threads(NUM1024, 1, 1);
	__global__ void MB2D_SlidingWindows_y(cufftComplex* complexData, uint32_t RawAsize, uint32_t ObjBsize, uint32_t AvgNsize) {
		int i;
		int index = blockIdx.z * RawAsize * ObjBsize + threadIdx.x;
		for (i = 0; i < ObjBsize - 1; i++) {
			complexData[index + i * RawAsize].x += complexData[index + (i + 1) * RawAsize].x;
			complexData[index + i * RawAsize].y += complexData[index + (i + 1) * RawAsize].y;
		}
		complexData[index + i * RawAsize].x += complexData[index + i * RawAsize].x;
		complexData[index + i * RawAsize].y += complexData[index + i * RawAsize].y;
		index = index + NUM1024;
		for (i = 0; i < ObjBsize - 1; i++) {
			complexData[index + i * RawAsize].x += complexData[index + (i + 1) * RawAsize].x;
		}
		complexData[index + i * RawAsize].x += complexData[index + i * RawAsize].x;
	}
	//=>dim3 blocks(1, 1, ObjBsize), threads(RawAsize / 32 = 64, 1, 1);
	__global__ void MB2D_OctaPosProcess(cufftComplex* complexData, uint32_t RawAsize, uint32_t ObjBsize, uint32_t AvgNsize) {
		int idx = threadIdx.x; //沿着aline方向
		int index = idx + blockIdx.z * RawAsize; //沿着aline、bline方向
		while (idx < (RawAsize / 2) && index < RawAsize * ObjBsize * AvgNsize)
		{
			uint32_t IdxAn = 0;
			for (int i = 0; i < AvgNsize - 1; i++) {
				IdxAn = index + NUM1024 + i * RawAsize * ObjBsize;
				float molecular = sqrtf(complexData[IdxAn - NUM1024].x * complexData[IdxAn - NUM1024].x + complexData[IdxAn - NUM1024].y * complexData[IdxAn - NUM1024].y); //求abs(complexData[IdxAn - NUM1024])
				float denominator = sqrtf(complexData[IdxAn].x) * sqrtf(complexData[IdxAn + RawAsize * ObjBsize].x);
				complexData[IdxAn - NUM1024].x = 1.00f - (molecular / denominator);
				complexData[IdxAn - NUM1024].y = 0;
			}
			idx += blockDim.x;   //+=(ascanSize/@) < ascanSize
			index += blockDim.x; //+=(ascanSize/@) < ascanSize * bscanSize * blockSize
		}
	}
	//=>dim3 blocks(1, 1, ObjBsize), threads(RawAsize/32, 1, 1); 
	__global__ void MB2D_DevDataTohosData(cufftComplex* complexData, float* dev_Data, uint32_t RawAsize, uint32_t ObjAsize, uint32_t ObjBsize, uint32_t AvgNsize) {
		//MB和BM扫描模式都适合
		int idx = threadIdx.x; //沿着aline方向
		int index1 = idx + blockIdx.z * RawAsize; //沿着aline、bline方向
		int index2 = idx + blockIdx.z * ObjAsize; //沿着aline、bline方向
		while (idx < ObjAsize)
		{
			float sum = 0.00f;
			for (int i = 0; i < AvgNsize - 1; i++) {
				int index = index1 + i * (RawAsize * ObjBsize);
				sum += sqrtf(complexData[index].x * complexData[index].x + complexData[index].y * complexData[index].y);
			}
			dev_Data[index2] = sum / (AvgNsize - 1.0f);
			idx += blockDim.x;
			index1 += blockDim.x;
			index2 += blockDim.x;
		}
	}
	//MB扫描模式下，复数去相关算法(CC算法)
	void MB2D_ComplexDeCorrAlgorithm(uint16_t* dev_rawApoData, uint16_t* hos_rawApoData, //ApoData
		uint16_t* dev_rawSpecData, uint16_t* hos_rawSpecData, //SpecData
		float* dev_apoData,  //ApoData in GPU，Preloaded On Initialization
		uint32_t AvgNsize, uint32_t ApoNsize,
		uint32_t ObjAsize, uint32_t ObjBsize,
		cufftComplex* dev_complexData, cufftComplex* dev_complexDataX, //midData
		cufinufftf_plan dplan, cudaStream_t stream, //cudaParam
		float* dev_outputData, float* hos_outputData) //outputData
	{
		//拷贝输入数据=>GPU显存
		cudaErrchk(cudaMemcpyAsync(dev_rawApoData, hos_rawApoData, NUM2048 * ApoNsize * AvgNsize * sizeof(uint16_t), cudaMemcpyHostToDevice, stream));
		cudaErrchk(cudaMemcpyAsync(dev_rawSpecData, hos_rawSpecData, NUM2048 * ObjBsize * AvgNsize * sizeof(uint16_t), cudaMemcpyHostToDevice, stream));
		//0、求Apo平均
		MB2D_GetApoMean << <dim3(1, 1, 8), dim3(256, 1, 1), 0, stream >> > (dev_apoData, dev_rawApoData, ApoNsize * AvgNsize);
		//1、减Apo、添加窗
		dim3 blocks1(AvgNsize, 1, ObjBsize), threads1(NUM2048 / 32, 1, 1);
		MB2D_ApoAndHann << <blocks1, threads1, 0, stream >> > (dev_complexDataX, dev_rawSpecData, dev_apoData, NUM2048, ObjBsize, AvgNsize);
		//2、执行cufinufft(非均匀傅里叶变换)
		int code = cufinufftf_execute(dev_complexDataX, dev_complexDataX, dplan);
		if (code != 0) fprintf(stderr, "NUFFTassert: %d %s %d\n", code, __FILE__, __LINE__);
		//3、转换维度[RawAsize,AvgNsize,ObjBsize]=>[RawAsize,ObjBsize,AvgNsize]
		dim3 blocks2(1, 1, ObjBsize), threads2(NUM2048 / 32, 1, 1);
		MB2D_OctXYZtoXZY << <blocks2, threads2, 0, stream >> > (dev_complexDataX, dev_complexData, NUM2048, AvgNsize, ObjBsize);
		//4、CC算法预处理
		MB2D_OctaProProcess << <blocks2, threads2, 0, stream >> > (dev_complexData, NUM2048, ObjBsize, AvgNsize);
		//5、拆分滑窗之列滑窗
		dim3 blocks3(1, 1, AvgNsize), threads3(ObjBsize, 1, 1);
		MB2D_SlidingWindows_x << <blocks3, threads3, 0, stream >> > (dev_complexData, NUM2048, ObjBsize, AvgNsize);
		//6、拆分滑窗之行滑窗
		dim3 blocks4(1, 1, AvgNsize), threads4(NUM1024, 1, 1);
		MB2D_SlidingWindows_y << <blocks4, threads4, 0, stream >> > (dev_complexData, NUM2048, ObjBsize, AvgNsize);
		//7、除法
		MB2D_OctaPosProcess << <blocks2, threads2, 0, stream >> > (dev_complexData, NUM2048, ObjBsize, AvgNsize);
		//8、求平均
		MB2D_DevDataTohosData << <dim3(1, 1, ObjBsize), dim3(NUM2048 / 32, 1, 1), 0, stream >> > (dev_complexData, dev_outputData, NUM2048, NUM1024, ObjBsize, AvgNsize);
		//拷贝结果数据=>CPU内存
		cudaErrchk(cudaMemcpyAsync(hos_outputData, dev_outputData, NUM1024 * ObjBsize * sizeof(float), cudaMemcpyDeviceToHost, stream));
	}
}