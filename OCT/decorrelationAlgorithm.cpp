#include "decorrelationAlgorithm.h"


/**
 * @brief DecorrelationAlgorithm类的构造函数
 * @param Z 输入参数Z，用于初始化成员变量Z
 * @param X 输入参数X，用于初始化成员变量X
 * @param m 输入参数m，用于初始化成员变量m
 * @param n 输入参数n，用于初始化成员变量n
 */
DecorrelationAlgorithm::DecorrelationAlgorithm(int X, int Z, int m, int n)
{
    // 初始化卷积输出向量
    this->conv2_same_output = vector<float>();
    // 初始化去相关结果向量
    this->decorrelation_result = vector<float>();
    // 初始化成员变量
    this->X = X;
    this->Z = Z;
    this->m = m;
    this->n = n;
}


/**
 * @brief 对输入数据执行二维卷积运算（same模式）
 * 
 * 该函数实现了一个二维卷积操作，使用"same"填充策略，
 * 输出尺寸与输入尺寸相同。卷积核在输入数据上滑动，
 * 计算局部区域与卷积核的加权和。
 * 
 * @param input 输入数据，按行优先顺序存储的二维数组，大小为 Z*X
 * @param kernel 卷积核，按行优先顺序存储的二维数组，大小为 m*n
 * 
 * @note 输出结果存储在类成员变量 conv2_same_output 中
 * @note 该函数假设类成员变量 Z、X、m、n 已正确初始化
 */
void DecorrelationAlgorithm:: conv2_same(const vector<float>& input, const vector<float>& kernel)
{
    int rows = this->X; int cols = this->Z; int k_rows = this->m; int k_cols = this->n;
    vector<float> output(rows * cols, 0.0f);
    
    // 计算卷积核的中心位置，用于确定卷积操作的对齐方式
    int k_center_r = k_rows / 2;
    int k_center_c = k_cols / 2;

    // 遍历输出的每个位置，计算对应的卷积值
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            float sum = 0.0f;
            
            // 在卷积核范围内进行卷积计算，遍历卷积核的每个元素
            for (int kr = 0; kr < k_rows; ++kr) {
                int rr = r + kr - k_center_r;
                // 边界检查，跳过超出输入范围的行
                if (rr < 0 || rr >= rows) continue;
                for (int kc = 0; kc < k_cols; ++kc) {
                    int cc = c + kc - k_center_c;
                    // 边界检查，跳过超出输入范围的列
                    if (cc < 0 || cc >= cols) continue;
                    // 累加输入值与卷积核值的乘积
                    sum += input[rr * cols + cc] * kernel[kr * k_cols + kc];
                }
            }
            output[r * cols + c] = sum;
        }
    }
    this->conv2_same_output = output;
}



/**
 * @brief 计算两个复数向量之间的去相关性（decorrelation），用于衡量信号之间相似性的降低程度。
 *
 * 该方法通过以下步骤实现：
 * 1. 对输入的复数向量 A 和 B 进行逐元素相乘（其中 B 取共轭）；
 * 2. 分离结果的实部和虚部，并分别进行滑动窗口平均处理；
 * 3. 计算上述结果的模长作为联合能量；
 * 4. 同时计算各自信号的能量并做滑动平均；
 * 5. 利用联合能量与独立能量之比，归一化得到去相关图。
 *
 * @param[in] A 第一个复数向量，表示前一帧数据（展平为一维）
 * @param[in] B 第二个复数向量，表示后一帧数据（展平为一维）
 */
void DecorrelationAlgorithm::compute_complex_decorrelation(
    const vector<complex<float>>& A, const vector<complex<float>>& B)
{
    int X = this->X; int Z = this->Z; int m = this->m; int n = this->n;
    int size = X * Z;
    vector<float> decor_map(size, 0.0f); // 存储某一时间步的单张去相关图

    // 初始化全1的滑动窗口核
    vector<float> kernel(m * n, 1.0f);

    // 步骤一：计算 A 与 B 共轭的逐元素相乘结果
    vector<complex<float>> AB_conj(size);
#pragma omp parallel for // 使用多线程
    for (int i = 0; i < size; ++i) 
    {
        // A和B分别是前后两帧图像压缩成的大小为（Z*X）的向量，即：frames[t][i] * frames[t+1][i]
        AB_conj[i] = A[i] * conj(B[i]);
    }

    // 步骤二：将复数结果分离为实部和虚部
    vector<float> AB_real(size), AB_imag(size);
#pragma omp parallel for // 使用多线程
    for (int i = 0; i < size; ++i) {
        AB_real[i] = AB_conj[i].real();
        AB_imag[i] = AB_conj[i].imag();
    }

    // 对实部和虚部分别执行滑动窗口平均（same边界模式的二维卷积）
    conv2_same(AB_real, kernel);
    vector<float> conv_real = this->conv2_same_output;
    conv2_same(AB_imag, kernel);
    vector<float> conv_imag = this->conv2_same_output;

    // 步骤三：计算复数幅值（即模长）
    vector<float> AB_len(size);
#pragma omp parallel for // 使用多线程
    for (int i = 0; i < size; ++i) {
        AB_len[i] = hypot(conv_real[i], conv_imag[i]); // hypot = sqrt(real² + imag²)，AB相乘后经滑动平均后的模长
    }

    // 步骤四：计算各自信号的能量（模的平方）
    vector<float> As(size), Bs(size);
#pragma omp parallel for
    for (int i = 0; i < size; ++i) {
        As[i] = norm(A[i]); // A的模长
        Bs[i] = norm(B[i]); // B的模长
    }

    // 对能量图像同样进行滑动窗口平均
    conv2_same(As, kernel);
    vector<float> conv_As = this->conv2_same_output;
    conv2_same(Bs, kernel);
    vector<float> conv_Bs = this->conv2_same_output;

    // 步骤五：开方得到幅值
    vector<float> A_len(size), B_len(size);
#pragma omp parallel for
    for (int i = 0; i < size; ++i) {
        A_len[i] = sqrt(conv_As[i]);
        B_len[i] = sqrt(conv_Bs[i]);
    }

    // 步骤六：归一化并计算最终去相关度量
    const float eps = 1e-6f;
#pragma omp parallel for // 使用多线程
    for (int i = 0; i < size; ++i) {
        float denom = A_len[i] * B_len[i] + eps;
        decor_map[i] = 1 - AB_len[i] / denom;
    }

    this->decorrelation_result = decor_map;
}
