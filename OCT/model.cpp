#include"model.h"
#include "decorrelationAlgorithm.h"
#include<opencv2/imgcodecs.hpp>
#include<opencv2/imgcodecs.hpp>

/**
 * @brief Model类的构造函数
 * @param parent 父对象指针，用于Qt对象树管理
 * 
 * 该构造函数初始化Model对象的所有成员变量，包括OCT文件处理、
 * 数据存储、扫描参数和结果指针等。所有数值型成员变量被初始化
 * 为0或默认值，指针成员变量被初始化为nullptr。
 */
Model::Model(QObject* parent) : QObject(parent) 
{
	
	// 初始化阈值参数
	Threshold = 45;
	//intensityImage = nullptr;
	
	// 初始化扫描范围参数
	Z_range = 0;
	X_range = 0;
	Y_range = 0;
	
	// 初始化像素参数
	Z_pixel = 0;
	X_pixel = 0;
	Y_pixel = 0;
	
	// 初始化扫描重复次数
	Ascan_repeat = 0;
	Bscan_repeat = 0;
	
	// 初始化扫描模式
	Scan_mode = "null";
	
	// 初始化强度数据指针
	Intensity_ptr = nullptr;
	
	// 初始化结果指针
	liv_ptr = nullptr;
	octa_result_ptr = nullptr;
	oct_intensity_mask = nullptr;
}



bool Model::OnError(void)
{
	char errorMessage[1024];
	if (getError(errorMessage, 1024))
	{
		qDebug() << "ERROR: " << errorMessage << "\n";
		return true;
	}
	return false;
}


//	从本地获取三维图像数据
void Model::getIntensityImage(QString& filePath) {
	OCTFileHandle octFile = createOCTFile(FileFormat_OCITY);
	loadFile(octFile, filePath.toStdString().c_str());

	// 获取元数据
	X_range = getFileMetadataFloat(octFile, FileMetadata_RangeX);
	Y_range = getFileMetadataFloat(octFile, FileMetadata_RangeY);
	Z_range = getFileMetadataFloat(octFile, FileMetadata_RangeZ);

	X_pixel = getFileMetadataInt(octFile, FileMetadata_SizeX);
	Y_pixel = getFileMetadataInt(octFile, FileMetadata_SizeY);
	Z_pixel = getFileMetadataInt(octFile, FileMetadata_SizeZ);

	// 获取数据
	DataHandle Intensity = createData();
	getFileRealData(octFile, Intensity, findFileDataObject(octFile, DataObjectName_OCTData));
	float* temp_ptr = getDataPtr(Intensity);  // 临时指针

	// 值拷贝：创建新的内存空间并复制数据
	int totalSize = X_pixel * Y_pixel * Z_pixel;
	if (this->Intensity_ptr != nullptr) {
		delete[] this->Intensity_ptr;  // 清理旧数据
		this->Intensity_ptr = nullptr;
	}
	this->Intensity_ptr = new float[totalSize];

	// 执行值拷贝
	memcpy(this->Intensity_ptr, temp_ptr, totalSize * sizeof(float));

	// 清理资源
	clearData(Intensity);  // 清理DataHandle
	clearOCTFile(octFile);
}



// 加载并处理 OCT 原始数据文件
void Model::loadAndProcessOCTRawData(const QString& fileName) {
	OCTFileHandle octFile = createOCTFile(FileFormat_OCITY);
	
	ComplexDataHandle Complex3D_data = createComplexData();
	
	loadFile(octFile, fileName.toStdString().c_str());

	// 获取 OCT 文件中的元数据信息：扫描范围与像素尺寸等
	X_range = getFileMetadataFloat(octFile, FileMetadata_RangeX);
	Y_range = getFileMetadataFloat(octFile, FileMetadata_RangeY);
	Z_range = getFileMetadataFloat(octFile, FileMetadata_RangeZ);

	X_pixel = getFileMetadataInt(octFile, FileMetadata_SizeX);
	Y_pixel = getFileMetadataInt(octFile, FileMetadata_SizeY);
	Z_pixel = getFileMetadataInt(octFile, FileMetadata_SizeZ);
	Ascan_repeat = getFileMetadataInt(octFile, FileMetadata_SpeckleAveragingFastAxis);
	Bscan_repeat = getFileMetadataInt(octFile, FileMetadata_SpeckleAveragingSlowAxis);
	Scan_mode = getFileMetadataString(octFile, FileMetadata_AcquisitionMode);


	ProcessingHandle Proc = createProcessingForOCTFile(octFile);

	// 按照 Y 方向像素数乘以 B 扫描重复次数逐帧读取并处理原始数据
	for (int i = 0; i < Y_pixel*Bscan_repeat; i++) 
	{
		RawDataHandle Temp_rawData = createRawData();
		ComplexDataHandle complex_temp = createComplexData();
		getFileRawData(octFile, Temp_rawData, i + findFileDataObject(octFile, "data\\Spectral0.data"));

		// 设置当前处理流程的输出目标为复数数据对象
		setComplexDataOutput(Proc, complex_temp);
		executeProcessing(Proc, Temp_rawData);

		// 将处理后的一帧复数数据追加至总的三维复数数据集中
		appendComplexData(Complex3D_data, complex_temp, Direction_3);

		clearRawData(Temp_rawData);
		clearComplexData(complex_temp);

		OnError();
	}

	// 获取处理完成后复数数据在 Y 轴方向上的实际大小
	int size_y = getComplexDataPropertyInt(Complex3D_data, Data_Size3);

	ComplexFloat* ComplexData_ptr = getComplexDataPtr(Complex3D_data);

	// 调整本地复数数据缓存大小，确保能容纳所有数据点
	complex_data.resize(Z_pixel * X_pixel * Y_pixel * Bscan_repeat);

	for (int i = 0; i < Z_pixel * X_pixel * Y_pixel * Bscan_repeat; ++i) {
		complex_data[i] = complex<float>(ComplexData_ptr[i].data[0], ComplexData_ptr[i].data[1]);
	}

	clearComplexData(Complex3D_data);
	clearOCTFile(octFile);
}


void Model::OCTA_algorithm_complex_correlaton() {

}



// 计算OCTA数据，基于输入的复数信号进行多帧互相关处理
float* Model::compute_OCTA(int m, int n, int interval)
{
	int X = this->X_pixel;
	int Z = this->Z_pixel;
	int Y = this->Y_pixel;
	int Repeat = this->Bscan_repeat;

	vector<float> result(X * Z * Y, 0.0f);

	// 并行处理每个Y层
#pragma omp parallel for
	for (int y = 0; y < Y; ++y)
	{
		// 将当前Y层的数据按帧拆分
		vector<vector<complex<float>>> frames(Repeat, vector<complex<float>>(X * Z));
		// time_len 表示每个Y位置上的帧数(即：时间序列长度)，frames的尺寸为：(time_len , X * Z)
		cout << y << endl;

		/* 拆分每帧数据:
		 * 该代码块将三维复数数据重新组织并存储到帧缓冲区中
		 *
		 * 外层三重循环遍历局部坐标系(r,z,x)的每个点
		 * 其中R、Z、X分别表示三个维度的大小
		 *
		 * 代码功能：
		 * - 将全局坐标y映射到局部坐标r
		 * - 通过行列优先的方式计算数据在原始数组中的索引
		 * - 将复数数据重新排列存储到frames中的二维数组中
		 *
		 * 数据映射关系：
		 * - true_y: 全局y坐标，由y和局部r坐标计算得出
		 * - idx: 原始complex_data数组中的一维索引
		 * - frames: 目标二维数组，按[r][z*x+x]方式存储
		 */

		for (int r = 0; r < Repeat; ++r) {
			for (int x = 0; x < X; ++x) {
				for (int z = 0; z < Z; ++z) {
					int true_y = y * Repeat + r;
					int idx = true_y * X * Z + x * Z + z;  // 保持原索引计算（y, x, z顺序）
					frames[r][x * Z + z] = this->complex_data[idx];  // 修改为x, z顺序
				}
			}
		}

		// 对每对间隔为interval的帧进行复数去相关计算
		vector<vector<float>> decor_maps; // 存储去相关图
		for (int i = 0; i < Repeat - interval; ++i)
		{
			// 创建去相关对象, 传入图像尺寸参数和滑动窗口尺寸参数
			DecorrelationAlgorithm decor_obj(X, Z, m, n);
			// 计算前后两帧的去相关图
			decor_obj.compute_complex_decorrelation(frames[i], frames[i + interval]);
			auto map = decor_obj.decorrelation_result;
			decor_maps.push_back(move(map));
		}

		// 对所有去相关图进行平均
		vector<float> avg_map(X * Z, 0.0f);
		for (int i = 0; i < decor_maps.size(); ++i) {
			for (int j = 0; j < X * Z; ++j) {
				avg_map[j] += decor_maps[i][j];
			}
		}
		for (int j = 0; j < X * Z; ++j) {
			avg_map[j] /= decor_maps.size();
		}

		// 将平均后的结果写入最终结果数组
		for (int x = 0; x < X; ++x) {
			for (int z = 0; z < Z; ++z) {
				int idx = (y * X + x) * Z + z;  // y, x, z顺序
				result[idx] = avg_map[x * Z + z];  // 修改为x, z顺序
			}
		}
	}

	// 将结果复制到动态分配的数组中并返回
	float* result_ptr = new float[X * Z * Y];
	copy(result.begin(), result.end(), result_ptr); // 复制数据
	return result_ptr;
}



/**
 * @brief 计算OCT强度掩膜（用于组织/血管分割）
 * @param threshold 强度阈值，用于区分组织和背景/血管区域,抑制组织和背景中的噪声对血管成像的影响
 *
 * 该函数执行以下主要步骤：
 * 1. 基于强度阈值生成初始二值掩膜
 * 2. 对掩膜进行3D形态学操作（先膨胀后腐蚀）以改善分割质量
 * 3. 使用OpenMP并行优化处理大规模3D数据
 */
void Model::compute_Intensity_mask(float threshold) {
	// 释放已存在的掩膜内存，防止内存泄漏
	if (oct_intensity_mask != nullptr) {
		delete[] oct_intensity_mask;
		oct_intensity_mask = nullptr;
	}

	// 为掩膜数据分配内存，尺寸为三维体积：X_pixel × Z_pixel × Y_pixel
	oct_intensity_mask = new uint8_t[X_pixel * Z_pixel * Y_pixel];

	// 使用OpenMP并行化三层嵌套循环，加速三维数据处理[6](@ref)[8](@ref)
#pragma omp parallel for collapse(3)
	for (int i = 0; i < Y_pixel; i++) {           // Y轴：B扫描数量（慢轴）
		for (int j = 0; j < X_pixel; j++) {       // X轴：B扫描宽度（快轴）
			for (int k = 0; k < Z_pixel; k++) {   // Z轴：A扫描深度
				float sum = 0;

				// 对每个空间位置(i,j,k)，累加所有B扫描重复测量的强度值
				for (int m = 0; m < Bscan_repeat; m++) {
					// 计算复数数据的幅度（dB值）：20*log10(|complex_data|)
					// 通过平均多次测量提高信噪比
					sum += 20 * (log10(abs(complex_data[(i * Bscan_repeat + m) * X_pixel * Z_pixel + j * Z_pixel + k])));
				}

				// 根据阈值生成二值掩膜：平均强度大于阈值设为255（前景），否则为0（背景）
				oct_intensity_mask[i * X_pixel * Z_pixel + j * Z_pixel + k] =
					uint8_t((sum / Bscan_repeat) > threshold ? 255 : 0);
			}
		}
	}

	// 三维形态学后处理：先膨胀后腐蚀（闭操作），消除噪声并平滑边界
	int r = 1; // 结构元素半径，定义3×3×3的立方体邻域
	uint8_t* temp_mask = new uint8_t[X_pixel * Z_pixel * Y_pixel];  // 临时缓冲区存储膨胀结果

	// Lambda函数：计算三维数据在一维数组中的索引[3](@ref)
	auto index = [&](int y, int x, int z) {
		return y * X_pixel * Z_pixel + x * Z_pixel + z;
		};

	// 三维膨胀操作：寻找每个体素邻域内的最大值[7](@ref)
#pragma omp parallel for collapse(3) // 
	for (int y = 0; y < Y_pixel; ++y) {
		for (int x = 0; x < X_pixel; ++x) {
			for (int z = 0; z < Z_pixel; ++z) {
				uint8_t max_val = 0;
				// 在3×3×3邻域内搜索最大值（膨胀操作）
				for (int dy = -r; dy <= r; ++dy) {
					for (int dx = -r; dx <= r; ++dx) {
						for (int dz = -r; dz <= r; ++dz) {
							int yy = y + dy, xx = x + dx, zz = z + dz;
							// 边界检查，确保索引有效
							if (yy >= 0 && yy < Y_pixel &&
								xx >= 0 && xx < X_pixel &&
								zz >= 0 && zz < Z_pixel) {
								max_val = max(max_val, oct_intensity_mask[index(yy, xx, zz)]);
							}
						}
					}
				}
				temp_mask[index(y, x, z)] = max_val;
			}
		}
	}

	// 三维腐蚀操作：寻找每个体素邻域内的最小值[7](@ref)
#pragma omp parallel for collapse(3)
	for (int y = 0; y < Y_pixel; ++y) {
		for (int x = 0; x < X_pixel; ++x) {
			for (int z = 0; z < Z_pixel; ++z) {
				uint8_t min_val = 255;
				// 在3×3×3邻域内搜索最小值（腐蚀操作）
				for (int dy = -r; dy <= r; ++dy) {
					for (int dx = -r; dx <= r; ++dx) {
						for (int dz = -r; dz <= r; ++dz) {
							int yy = y + dy, xx = x + dx, zz = z + dz;
							// 边界检查，确保索引有效
							if (yy >= 0 && yy < Y_pixel &&
								xx >= 0 && xx < X_pixel &&
								zz >= 0 && zz < Z_pixel) {
								min_val = min(min_val, temp_mask[index(yy, xx, zz)]);
							}
						}
					}
				}
				// 将腐蚀结果存回原掩膜数组
				oct_intensity_mask[index(y, x, z)] = min_val;
			}
		}
	}

	// 释放临时缓冲区内存
	delete[] temp_mask;
}














