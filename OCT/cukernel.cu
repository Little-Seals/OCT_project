#include "cukernel.cuh"
/* 构造函数 default */
CudaProcess::CudaProcess() { //构造函数
	//this->ObjAsize = 1024;
	//this->ObjBsize = 400;
	//this->ScanMode = 0; //MB
	//this->AvgNsize = 4;
	//this->RawAsize = (this->ObjAsize) * 2;
	//this->RawBsize = (this->ObjBsize) * (this->AvgNsize);
	//cudaErrchk(cudaEventCreate(&this->start));
	//cudaErrchk(cudaEventCreate(&this->stop));
	//this->CudaMemMalloc();
}
/* 构造函数 params */
CudaProcess::CudaProcess(uint32_t AvgNsize, uint32_t ApoNsize, uint32_t ObjAsize, uint32_t ObjBsize, bool ScanMode) :
	AvgNsize(AvgNsize), ApoNsize(ApoNsize), ObjAsize(ObjAsize), ObjBsize(ObjBsize), ScanMode(ScanMode)
{
	//切趾光谱尺寸
	this->RawApoAsize = ObjAsize * 2;
	this->RawApoBsize = ApoNsize * AvgNsize;
	//信号光谱尺寸
	this->RawSpecAsize = ObjAsize * 2;
	this->RawSpecBsize = ObjBsize * AvgNsize;

	printf("size(RawApoData)=[%d,%d]\n", (this->RawApoAsize), (this->RawApoBsize));
	printf("size(RawSpecData)=[%d,%d]\n", (this->RawSpecAsize), (this->RawSpecBsize));
	cudaErrchk(cudaEventCreate(&this->start));
	cudaErrchk(cudaEventCreate(&this->stop));
	this->CudaMemMalloc();
}
/* 加载校准数据 GPU */
void CudaProcess::CudaLoadComData(float*& hos_chirpData) {
	if (dev_chirpData == nullptr) {
		printf("Err:chirpData not malloc %s %d", __FILE__, __LINE__); return;
	}
	for (int i = 0; i < RawSpecAsize; i++) {
		hos_chirpData[i] = M_PI * (hos_chirpData[i] / (RawSpecAsize / 2.0) - 1.0f);
	}
	cudaErrchk(cudaMemcpy(dev_chirpData, hos_chirpData, RawSpecAsize * sizeof(float), cudaMemcpyHostToDevice));
	nufftErrchk(cufinufftf_setpts(RawSpecAsize, dev_chirpData, NULL, NULL, 0, NULL, NULL, NULL, dplan));
}
/* 申请GPU缓存 */
void CudaProcess::CudaMemMalloc() {
	int ier;
	//Apo数据、采样数据、光谱数据、缓存数据
	cudaErrchk(cudaMalloc((void**)&dev_apoData, RawApoAsize * sizeof(float)));
	cudaErrchk(cudaMalloc((void**)&dev_chirpData, RawApoAsize * sizeof(float)));
	cudaErrchk(cudaMalloc((void**)&dev_rawApoData, RawApoAsize * RawApoBsize * sizeof(uint16_t)));
	cudaErrchk(cudaMalloc((void**)&dev_rawSpecData, RawSpecAsize * RawSpecBsize * sizeof(uint16_t)));
	cudaErrchk(cudaMalloc((void**)&dev_complexData, RawSpecAsize * RawSpecBsize * sizeof(cufftComplex)));
	cudaErrchk(cudaMalloc((void**)&dev_complexDataX, RawSpecAsize * RawSpecBsize * sizeof(cufftComplex)));
	cudaErrchk(cudaMalloc((void**)&dev_retfloatData, ObjAsize * ObjBsize * sizeof(float)));
	//申请NUFFT资源
	const int type = 1;
	nufftErrchk(cufinufftf_default_opts(type, 1, &opts)); //(type、dim、opts)
	int nmodes[3] = { RawSpecAsize,1,1 };
	int ntransf = RawSpecBsize;
	int maxbatchsize = 8;
	nufftErrchk(cufinufftf_makeplan(type, 1, nmodes, 1, ntransf, 1e-6, maxbatchsize, &dplan, &opts));
}
/* 释放GPU缓存 */
void CudaProcess::CudaMemDelete() {
	nufftErrchk(cufinufftf_destroy(this->dplan));
	if (dev_apoData != nullptr) cudaErrchk(cudaFree(dev_apoData));
	if (dev_chirpData != nullptr) cudaErrchk(cudaFree(dev_chirpData));
	if (dev_rawApoData != nullptr) cudaErrchk(cudaFree(dev_rawApoData));
	if (dev_rawSpecData != nullptr) cudaErrchk(cudaFree(dev_rawSpecData));
	if (dev_complexData != nullptr) cudaErrchk(cudaFree(dev_complexData));
	if (dev_complexDataX != nullptr) cudaErrchk(cudaFree(dev_complexDataX));
	if (dev_retfloatData != nullptr) cudaErrchk(cudaFree(dev_retfloatData));
}
/* 析构函数 */
CudaProcess::~CudaProcess() { //析构函数
	this->CudaMemDelete();
}
/* Nufft Err Check */
void nufftAssert(int code, const char* file, int line)
{
	if (code != 0)
	{
		fprintf(stderr, "NUFFTassert: %d %s %d\n", code, file, line);
	}
}

/* 处理OCT数据 */
void CudaProcess::RunOctPro(uint16_t* hos_inputData, float* hos_retData) {
}


/* 处理OCT数据 */
void CudaProcess::RunOctaMB2D(uint16_t* hos_rawApoData, uint16_t* hos_rawSpecData, float* hos_retfloatData) {
	//拷贝:CPU=>GPU
	cudaErrchk(cudaEventRecord(this->start, this->stream));
	MB2D_ComplexDeCorrAlgorithm(this->dev_rawApoData, hos_rawApoData, //RawApoData
		this->dev_rawSpecData, hos_rawSpecData, //RawSpecData
		this->dev_apoData,  //ApoData in GPU，Preloaded On Initialization
		this->AvgNsize, this->ApoNsize, //Avg And Apo Size
		this->ObjAsize, this->ObjBsize, //ObjAsize And ObjBsize
		this->dev_complexData, this->dev_complexDataX, //midData
		this->dplan, this->stream, //cudaParam
		this->dev_retfloatData, hos_retfloatData); //outputData
	cudaErrchk(cudaEventRecord(this->stop, this->stream))
}