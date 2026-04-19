# OCT 图像处理系统

## 项目简介
本项目是一个基于 Qt 和 C++ 的光学相干断层扫描（OCT）图像处理与分析系统。该系统支持加载 Thorlabs 格式的 `.oct` 原始数据，进行信号处理、结构成像以及光学相干断层血管成像（OCTA）。系统利用 OpenCV 进行图像可视化与后处理，并集成了 CUDA 加速算法以提升大数据量下的处理效率。

## 核心功能
- **OCT 原始数据加载**：支持解析 Thorlabs `.oct` 格式文件，读取复数信号及强度数据。
- **结构成像 (Structural OCT)**：从复数信号中提取组织结构的 B-scan 图像。
- **OCTA 血流成像**：基于复数去相关算法（Complex Decorrelation Algorithm）计算微血管血流信息。
- **强度掩膜 (Intensity Mask)**：通过阈值分割与形态学操作提取感兴趣区域（ROI），抑制背景噪声。
- **多模式显示**：支持 JET, VIRIDIS 等多种伪彩色映射方案。
- **批处理功能**：支持批量导出结构图与血流图的 PNG 序列。
- **GPU 加速**：集成 cufinufft 库实现非均匀快速傅里叶变换（NUFFT）的 GPU 加速处理。

## 环境要求与安装
### 1. 基础软件
- **操作系统**: Windows 10/11 (x64)
- **IDE**: Visual Studio 2022 (建议安装 C++ 桌面开发工作负载)
- **Qt 框架**: 
  - Debug 模式: Qt 6.6.3 (msvc2019_64)
  - Release 模式: Qt 6.8.3 (msvc2022_64)
  - 需安装模块: `core`, `gui`, `widgets`
- **CUDA Toolkit**: v12.6 (用于 GPU 加速处理)

### 2. 依赖库配置
请在项目根目录下创建 `Dependencies` 文件夹，并按以下结构放置第三方库：
```
OCT_project/
└── Dependencies/
    ├── opencv/          # OpenCV 4.11.0
    │   └── build/
    ├── SpectralRadar/   # Thorlabs Spectral Radar SDK
    │   ├── include/
    │   └── lib/
    └── Common/          # 其他通用头文件
```

### 3. 环境变量设置
为了确保程序能正确找到动态链接库（DLL），请将以下路径添加到系统环境变量 `Path` 中：
- Thorlabs SDK 的 `bin` 目录
- OpenCV 的 `x64/vc16/bin` 目录
- CUDA 的 `bin` 目录

### 4. 硬件注册
- **Thorlabs 设备注册**：双击运行 `Thorlabs.reg` 文件以注册必要的驱动与服务。

## 编译与运行
1. 使用 Visual Studio 2022 打开 `OCT.sln` 解决方案文件。
2. 在“解决方案资源管理器”中右键点击 `OCT` 项目，选择“设为启动项目”。
3. 确认构建配置（Debug/Release）与平台（x64）。
4. 点击“生成” -> “生成解决方案”进行编译。
5. 编译成功后，按 `F5` 或点击“本地 Windows 调试器”运行程序。

## 使用说明
### 数据加载
- **加载 RawData**：点击左上角“加载RawData”按钮，选择 `.oct` 文件。系统将自动解析元数据（FOV, Pixel, Scan Mode）。
- **加载强度数据**：若只需查看已处理的强度图，可使用“加载强度数据”按钮。

### 图像处理
- **切片浏览**：拖动底部的滑块可以沿 Y 轴（慢轴）浏览不同的 B-scan 截面。
- **应用 OCTA 算法**：在右侧“OCTA 计算结果”面板中，设置参数 `m`, `n`, `interval`，点击“应用OCTA算法”即可生成血流图。
- **计算强度掩膜**：在“强度掩膜”面板中调整阈值，点击“计算强度掩膜”以去除背景噪声。

### 数据导出
- **单帧保存**：使用“保存为 PNG”或“保存OCTA数据”按钮导出当前视图。
- **批量导出**：在右下角“批处理工作空间”中，选择文件夹可批量将 `.oct` 文件转换为 PNG 图片序列。

## 测试数据
测试用的 `.oct` 文件位于 `TestData` 文件夹中（如果存在）。

## 技术栈
- **前端界面**: Qt 6 (Widgets)
- **图像处理**: OpenCV 4.11.0
- **科学计算**: Thorlabs Spectral Radar SDK, cufinufft
- **并行计算**: CUDA, OpenMP
