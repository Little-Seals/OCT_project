#pragma once
#include <QObject>
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include "model.h"
#include "OCTGUI.h"
#include "save_function.h"
#include "windowDisplay.h"
#include <QProgressDialog>
//#include<Qstring>

/**
 * @brief 控制器类，用于连接模型和视图，处理用户交互事件
 * 
 * 该类继承自QObject，实现了MVC架构中的控制器部分，
 * 负责响应UI事件并更新模型数据
 */
class Controller : public QObject {
	Q_OBJECT // Qt宏，启用对象槽机制
public:
	Controller(Model* model, OCTGUI* view, QObject* parent = nullptr);
	~Controller();
	
	void saveoctaData(QString filename);
	
	void saveoctIntensityPng(QString filename);


private slots:
	
	void onLoadClicked();
	
	
	void onSliderValueChanged(int value);

	
	
	void onOCTAbuttonClicked();

	
	void onOCTASliderValueChanged(int value);

	
	void onOCTAContrastMaxSliderChanged(int value);
	
	
	void onOCTAContrastMinSliderChanged(int value);

	
	void onMaskButtonClicked();

	
	void onSaveOCTAButtonClicked();

	
	void onOctMaskSliderChanged(int value);

	
	void onThresholdSpinChanged(double threshold);
	
	void onButtonsaveOCTStructure();
	
	void onBatchexportIntensitywithintensitydata();
	
	void onButton_batch_export_octaAndIntensity();

	void onLoadIntensityClicked();  // 新增：加载强度数据

	void IntensitySliderValueChanged(int value);

	void onBatchSaveIntensityClicked();


private:
	Model* model;  ///< 数据模型指针
	OCTGUI* view;  ///< 用户界面视图指针
    WindowDisplay* ws;
	cv::Mat generateIntensitySliceImage(int sliceIndex);  // 新增：生成强度数据图像
	void batchSaveIntensityAsPng(const QString& outputDir);
	void setUIStateForIntensityMode();
	void setUIStateForRawDataMode();
};