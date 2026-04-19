#include"controller.h"

/**
 * @brief Controller类的构造函数，用于初始化控制器对象并设置视图与模型之间的连接。
 *
 * 在该构造函数中完成以下操作：
 * 1. 初始化成员变量 model 和 view；
 * 2. 设置界面控件（如滑动条、下拉框）的初始状态；
 * 3. 添加颜色映射选项到组合框；
 * 4. 设置默认的颜色映射项；
 * 5. 连接视图中的UI控件信号至控制器对应的槽函数。
 *
 * @param m 指向Model对象的指针，表示数据模型。
 * @param v 指向OCTGUI对象的指针，表示用户界面。
 * @param parent QObject父对象，用于内存管理和事件传递，默认为nullptr。
 */
Controller::Controller(Model* m, OCTGUI* v, QObject* parent)
	:QObject(parent), model(m), view(v), ws(new WindowDisplay())
{
	// 设置阈值双精度旋钮的默认值为45
	view->ui.doubleSpin_threshold->setValue(45);

	// 配置水平滑动条的最小值、最大值和当前值
	view->ui.horizontalSlider->setMinimum(0);
	view->ui.horizontalSlider->setMaximum(model->Y_pixel-1);
	view->ui.horizontalSlider->setValue(0);

	// 向comboBox添加OpenCV支持的各种颜色映射模式
	view->ui.comboBox->addItem("AUTUMN", cv::COLORMAP_AUTUMN);
	view->ui.comboBox->addItem("BONE", cv::COLORMAP_BONE);
	view->ui.comboBox->addItem("JET", cv::COLORMAP_JET);
	view->ui.comboBox->addItem("WINTER", cv::COLORMAP_WINTER);
	view->ui.comboBox->addItem("RAINBOW", cv::COLORMAP_RAINBOW);
	view->ui.comboBox->addItem("OCEAN", cv::COLORMAP_OCEAN);
	view->ui.comboBox->addItem("SUMMER", cv::COLORMAP_SUMMER);
	view->ui.comboBox->addItem("SPRING", cv::COLORMAP_SPRING);
	view->ui.comboBox->addItem("COOL", cv::COLORMAP_COOL);
	view->ui.comboBox->addItem("HSV", cv::COLORMAP_HSV);
	view->ui.comboBox->addItem("PINK", cv::COLORMAP_PINK);
	view->ui.comboBox->addItem("HOT", cv::COLORMAP_HOT);
	view->ui.comboBox->addItem("PARULA", cv::COLORMAP_PARULA);
	view->ui.comboBox->addItem("MAGMA", cv::COLORMAP_MAGMA);
	view->ui.comboBox->addItem("INFERNO", cv::COLORMAP_INFERNO);
	view->ui.comboBox->addItem("PLASMA", cv::COLORMAP_PLASMA);
	view->ui.comboBox->addItem("VIRIDIS", cv::COLORMAP_VIRIDIS);
	view->ui.comboBox->addItem("CIVIDIS", cv::COLORMAP_CIVIDIS);
	view->ui.comboBox->addItem("TWILIGHT", cv::COLORMAP_TWILIGHT);
	view->ui.comboBox->addItem("TWILIGHT_SHIFTED", cv::COLORMAP_TWILIGHT_SHIFTED);
	view->ui.comboBox->addItem("TURBO", cv::COLORMAP_TURBO);
	view->ui.comboBox->addItem("DEEPGREEN", cv::COLORMAP_DEEPGREEN);

	// 设置默认选中的颜色映射为"JET"
	int index = view->ui.comboBox->findText("JET");
	if (index != -1) {
		view->ui.comboBox->setCurrentIndex(index);
	}

	// 建立各个UI控件与对应处理函数之间的信号-槽连接关系
	QObject::connect(view->ui.loadButton, &QPushButton::clicked, this, &Controller::onLoadClicked);
	QObject::connect(view->ui.horizontalSlider, &QSlider::valueChanged, this, &Controller::onSliderValueChanged);
	QObject::connect(view->ui.OCTAButton, &QPushButton::clicked, this, &Controller::onOCTAbuttonClicked);
	// QObject::connect(view->ui.ShowOCTAbutton, &QPushButton::clicked, this, &Controller::onOCTAshowButtonClicked);
	QObject::connect(view->ui.OCTA_slider, &QSlider::valueChanged, this, &Controller::onOCTASliderValueChanged);
	QObject::connect(view->ui.slider_min_octa_value, &QSlider::valueChanged, this, &Controller::onOCTAContrastMinSliderChanged);
	QObject::connect(view->ui.slider_max_octa_value, &QSlider::valueChanged, this, &Controller::onOCTAContrastMaxSliderChanged);
	QObject::connect(view->ui.pushButton_show_mask, &QPushButton::clicked, this, &Controller::onMaskButtonClicked);
	QObject::connect(view->ui.button_saveocta_data, &QPushButton::clicked, this, &Controller::onSaveOCTAButtonClicked);
	QObject::connect(view->ui.slider_intensity_mask, &QSlider::valueChanged, this, &Controller::onOctMaskSliderChanged);
	QObject::connect(view->ui.doubleSpin_threshold, &QDoubleSpinBox::valueChanged, this, &Controller::onThresholdSpinChanged);
	QObject::connect(view->ui.button_save_structure, &QPushButton::clicked, this, &Controller::onButtonsaveOCTStructure);
	QObject::connect(view->ui.button_batch_export_PNG, &QPushButton::clicked, this, &Controller::onBatchexportIntensitywithintensitydata);
	QObject::connect(view->ui.button_batch_export_PNG_2, &QPushButton::clicked, this, &Controller::onButton_batch_export_octaAndIntensity);

	// 将horizontalSlider和slider_intensity_mask建立双向同步连接
    QObject::connect(view->ui.horizontalSlider, &QSlider::valueChanged, view->ui.slider_intensity_mask, &QSlider::setValue);
    QObject::connect(view->ui.slider_intensity_mask, &QSlider::valueChanged, view->ui.horizontalSlider, &QSlider::setValue);
    QObject::connect(view->ui.horizontalSlider, &QSlider::valueChanged, view->ui.OCTA_slider, &QSlider::setValue);
    QObject::connect(view->ui.OCTA_slider, &QSlider::valueChanged, view->ui.horizontalSlider, &QSlider::setValue);
	
	// 新增：强度数据加载按钮和滑块的连接
    connect(view->ui.intensityButton, &QPushButton::clicked, this, &Controller::onLoadIntensityClicked);
    connect(view->ui.intensityDataSlider, &QSlider::valueChanged, this, &Controller::IntensitySliderValueChanged);

	// 新增：强度数据保存按钮连接
	connect(view->ui.saveIntensityButton, &QPushButton::clicked,
		this, &Controller::onBatchSaveIntensityClicked);
}


Controller::~Controller()
{
	if (ws != nullptr) delete ws;
	if (model != nullptr) delete model;
	if (view != nullptr) delete view;
}



// 处理用户点击“加载”按钮事件，用于打开并处理 OCT 图像文件。
void Controller::onLoadClicked() 
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, "please select oct files", "", "Images(*.png *.oct *.bmp");

    if (!fileName.isEmpty()) {
		this->setUIStateForRawDataMode();
        // 释放之前分配的内存资源，防止内存泄漏
        if (model->Intensity_ptr != nullptr) {
            delete[] model->Intensity_ptr;
            model->Intensity_ptr = nullptr;
        }
        if (model->octa_result_ptr != nullptr) {
            delete[] model->octa_result_ptr;
            model->octa_result_ptr = nullptr;
        }
        if (model->oct_intensity_mask != nullptr) {
            delete[] model->oct_intensity_mask;
            model->oct_intensity_mask = nullptr;
        }

        // 加载并处理选定的 OCT 原始数据
        model->loadAndProcessOCTRawData(fileName);

        // 更新界面上的元数据显示标签
        view->ui.label_scanMode->setText(QString::fromStdString(model->Scan_mode));
        view->ui.label_fov->setText(
            QString::number(model->Z_range) + "mm" + " x " +
            QString::number(model->X_range) + "mm" + " x " +
            QString::number(model->Y_range) + "mm"
        );
        view->ui.label_imageSize->setText(
            QString::number(model->Z_pixel) + "piexl" + " x " +
            QString::number(model->X_pixel) + "piexl" + " x " +
            QString::number(model->Y_pixel) + "piexl"
        );

        view->ui.label_Ascan_repeat->setText(QString::number(model->Ascan_repeat));
        view->ui.label_Bscan_repeat->setText(QString::number(model->Bscan_repeat));

        // 配置横向滑块的最小最大值范围
        view->ui.horizontalSlider->setMinimum(0);
        view->ui.horizontalSlider->setMaximum(model->Y_pixel - 1);

        // 配置 OCTA 滑块的取值范围
        view->ui.OCTA_slider->setMinimum(0);
        view->ui.OCTA_slider->setMaximum(model->Y_pixel - 1);

		view->ui.slider_intensity_mask->setMaximum(model->Y_pixel - 1);
		view->ui.slider_intensity_mask->setMinimum(0);

        // 初始化对比度控制滑块及其数值显示标签
        view->ui.slider_min_octa_value->setValue(0);
        view->ui.slider_max_octa_value->setValue(1000);

        view->ui.slider_min_octa_value->setMinimum(0);
        view->ui.slider_min_octa_value->setMaximum(1000);

        view->ui.slider_max_octa_value->setMinimum(0);
        view->ui.slider_max_octa_value->setMaximum(1000);

        view->ui.label_octa_minvalue->setText(QString::number(view->ui.slider_min_octa_value->value() / 1000.0f));
        view->ui.label_octa_maxvalue->setText(QString::number(view->ui.slider_max_octa_value->value() / 1000.0f));

		ws->BscanImagePocessor(model, view, "RawDataView", view->ui.horizontalSlider->value());
    }
}


// 处理滑块值变化事件的回调函数
void Controller::onSliderValueChanged(int value) 
{
	ws->BscanImagePocessor(model, view, "RawDataView", value);
}



// 该函数响应 OCTA 按钮点击事件，执行OCTA计算，并将结果以图像形式显示在界面上。
void Controller::onOCTAbuttonClicked() {
	if (model->octa_result_ptr != nullptr) {
		delete[] model->octa_result_ptr;
		model->octa_result_ptr = nullptr;
	}
	// 调用模型的 OCTA 计算方法，获取结果指针
	model->octa_result_ptr = model->compute_OCTA(
		view->ui.spinBox_gm_2->value(),
		view->ui.spinBox_gn_2->value(),
		view->ui.spinBox_gi_2->value()
	);

	if (model->octa_result_ptr != nullptr) {
		ws->BscanImagePocessor(model, view, "OCTAView", view->ui.OCTA_slider->value(), true);
	}
}



// Controller::onOCTASliderValueChanged 处理 OCTA 滑块值变化事件，更新并显示对应的 OCTA 图像
void Controller::onOCTASliderValueChanged(int value)
{
	if (model->octa_result_ptr != nullptr) 
	{
		ws->BscanImagePocessor(model, view, "OCTAView", value, true);
	}
}


void Controller::onOCTAContrastMinSliderChanged(int value) {

	view->ui.label_octa_minvalue->setText(QString::number(view->ui.slider_min_octa_value->value() / 1000.0f));
	view->ui.label_octa_maxvalue->setText(QString::number(view->ui.slider_max_octa_value->value() / 1000.0f));

}


void Controller::onOCTAContrastMaxSliderChanged(int value) {
	view->ui.label_octa_minvalue->setText(QString::number(view->ui.slider_min_octa_value->value() / 1000.0f));
	view->ui.label_octa_maxvalue->setText(QString::number(view->ui.slider_max_octa_value->value() / 1000.0f));

}


void Controller::onMaskButtonClicked() {
	model->compute_Intensity_mask(model->Threshold);

	// 设置滑块范围
	view->ui.slider_intensity_mask->setMaximum(model->Y_pixel - 1);
	view->ui.slider_intensity_mask->setMinimum(0);

	// 使用封装后的函数显示掩膜图像
	ws->displayIntensityMaskImage(model, view, view->ui.slider_intensity_mask->value());
}


void Controller::onOctMaskSliderChanged(int value) {
	if (model->oct_intensity_mask != nullptr) {
		// 使用封装后的函数显示掩膜图像
		ws->displayIntensityMaskImage(model, view, value);
	}
}



//save file function
template <typename T>
void saveRaw(const std::string& filename, const T* data, size_t numElements) {
	std::ofstream out(filename, std::ios::binary);
	if (!out) {
		std::cerr << "Failed to open " << filename << std::endl;
		return;
	}
	out.write(reinterpret_cast<const char*>(data), numElements * sizeof(T));
	out.close();
}


// 修改后的 saveOCTSlicesAsPNG 函数，添加进度回调
void saveOCTSlicesAsPNG(float* data, int Z, int X, int Y, const std::string& baseFilename, 
                        std::function<void(int, int)> progressCallback = nullptr) 
{
    // 1. 查找全局最小最大值
    float globalMin = data[0];
    float globalMax = data[0];
    int totalSize = Z * X * Y;
    
    for (int i = 1; i < totalSize; ++i) {
        if (data[i] < globalMin) globalMin = data[i];
        if (data[i] > globalMax) globalMax = data[i];
    }
    float range = globalMax - globalMin;
    if (range == 0) range = 1e-5f;

    // 2. 为每个Y切片保存PNG
    for (int y = 0; y < Y; ++y) {
        cv::Mat slice(Z, X, CV_8U);
        
        for (int x = 0; x < X; ++x) {
            for (int z = 0; z < Z; ++z) {
                int idx = Z * X * y + x * Z + z;
                float val = data[idx];
                int pixel = static_cast<int>(255.0f * (val - globalMin) / range);
                pixel = std::clamp(pixel, 0, 255);
                slice.at<uchar>(z, x) = static_cast<uchar>(pixel);
            }
        }
        
        std::ostringstream oss;
        oss << baseFilename << std::setw(4) << std::setfill('0') << y + 1 << ".png";
        cv::imwrite(oss.str(), slice);
        
        // 调用进度回调函数
        if (progressCallback) {
            progressCallback(y + 1, Y);
        }
    }
}



/**
 * @brief 处理保存 OCTA 数据按钮点击事件
 *
 * 当用户点击保存 OCTA 数据按钮时调用此函数。该函数会弹出文件保存对话框，
 * 让用户选择保存路径及文件名，并根据所选文件扩展名（.raw 或 .png）将 OCTA
 * 结果数据以相应格式保存到磁盘。
 *
 * 对于 .raw 文件：将三维 OCTA 数据保存为原始浮点数数组。
 * 对于 .png 文件：将每个 Z 层切片保存为 PNG 图像文件。
 *
 * 在保存前会对数据进行掩膜处理，仅保留有效区域的数据。
 */
void Controller::onSaveOCTAButtonClicked() {
    // 弹出文件保存对话框，获取用户指定的文件路径
    QString filename = QFileDialog::getSaveFileName(
        nullptr,
        "Please select the file path ",
        "./data",  // 默认路径
        "OCT Raw data (*.raw);;PNG image (*.png)"
    );

    if (!filename.isEmpty()) {
        // 若用户选择了 .raw 格式文件，则保存为原始数据格式
        if (filename.endsWith(".raw")) {

            // 分配内存用于存储掩膜后的 OCTA 数据
            float* octa_masked = new float[model->Z_pixel * model->X_pixel * model->Y_pixel];

            // 应用掩膜：只保留强度掩膜中大于 0 的位置对应的 OCTA 值
            for (int i = 0; i < model->Z_pixel * model->X_pixel * model->Y_pixel; i++) {
                if (model->oct_intensity_mask[i] > 0) {
                    octa_masked[i] = model->octa_result_ptr[i];
                }
                else {
                    octa_masked[i] = 0;
                }
            }

            // 调用保存函数将数据写入 .raw 文件
            saveRaw(filename.toStdString(), octa_masked, model->Z_pixel * model->X_pixel * model->Y_pixel);
            
            // 释放动态分配的内存
            delete[] octa_masked;
        }

        // 若用户选择了 .png 格式文件，则按切片保存为 PNG 图像
        if (filename.endsWith(".png")) {
            // 移除文件名中的 ".png" 扩展名部分，便于后续构造图像文件名
            filename.chop(4);

            // 分配内存用于存储掩膜后的 OCTA 数据
            float* octa_masked = new float[model->Z_pixel * model->X_pixel * model->Y_pixel];


            // 应用掩膜：只保留强度掩膜中大于 0 的位置对应的 OCTA 值
            for (int i = 0; i < model->Z_pixel * model->X_pixel * model->Y_pixel; i++) {
				int data_index = i;
                if (model->oct_intensity_mask[data_index] > 0) {
                    octa_masked[i] = model->octa_result_ptr[data_index];
                }
                else {
                    octa_masked[i] = 0;
                }
            }

            // 调用函数将每层切片保存为 PNG 图像
            saveOCTSlicesAsPNG(octa_masked, model->Z_pixel, model->X_pixel, model->Y_pixel, filename.toStdString());
            
            // 释放动态分配的内存
            delete[] octa_masked;
        }
    }
}



void Controller::onThresholdSpinChanged(double threshold) {
	model->Threshold = threshold;
}


void Controller::onButtonsaveOCTStructure() {
	QString filename = QFileDialog::getSaveFileName(
		nullptr,
		"Please select the file path ",
		"./structure",  // Default path
		"PNG image (*.png)"
	);

		if (filename.endsWith(".png")) {
			filename.chop(4);

			float* oct_intensity_ptr = new float[model->Z_pixel * model->X_pixel * model->Y_pixel];


			for (int i = 0; i < model->Z_pixel * model->X_pixel * model->Y_pixel; i++) {
				float magnitude = std::abs(model->complex_data[i]);
				oct_intensity_ptr[i] = float(20.0 * std::log10(magnitude + 10e-6f));
			}


			saveOCTSlicesAsPNG(oct_intensity_ptr, model->Z_pixel, model->X_pixel, model->Y_pixel, filename.toStdString());
			delete[] oct_intensity_ptr;
		}
	
}


void Controller::onBatchexportIntensitywithintensitydata() {
	QString folderPath = QFileDialog::getExistingDirectory(
		nullptr,                            // parent window
		"Please select a folder",          // the title of the window
		".",                                // the strater folder path this time is the current path
		QFileDialog::ShowDirsOnly          // show the folder only
	);

	if (folderPath.isEmpty()) {
		return;
	}


	std::string folder = folderPath.toStdString();
	std::string resultRoot = folder + "\\PNG_intensity";

	for (const auto& entry : std::filesystem::directory_iterator(folder)) {
		if (entry.path().extension() == ".oct") {
			std::string fullPath = entry.path().string();
			std::string fileNameNoExt = entry.path().stem().string();
			std::string outputFolder = resultRoot + "\\" + fileNameNoExt;

			//std::cout << "Processing: " << fullPath << "\n";
			view->ui.label_intensity_progress->setText(QString::fromStdString(fileNameNoExt));
			QApplication::processEvents();
			processOCTFile(fullPath, outputFolder);
		}
	}

	view->ui.label_intensity_progress->setText("Conversion Completely");
	QApplication::processEvents();

}


void Controller::onButton_batch_export_octaAndIntensity() {


	QString folderPath = QFileDialog::getExistingDirectory(
		nullptr,                            // parent window
		"Please select a folder",          // the title of the window
		".",                                // the strater folder path this time is the current path
		QFileDialog::ShowDirsOnly          // show the folder only
	);

	if (folderPath.isEmpty()) { return; }

	std::string folder = folderPath.toStdString();
	std::string resultRoot = folder + "\\result";

	for (const auto& entry : std::filesystem::directory_iterator(folder)) {
		if (entry.path().extension() == ".oct") {
			std::string fullPath = entry.path().string();
			std::string fileNameNoExt = entry.path().stem().string();
			std::string octa_outputFolder = resultRoot + "\\" +"octa\\"+ fileNameNoExt;
			std::string oct_intensityFolder = resultRoot + "\\" + "oct_intensity\\" + fileNameNoExt;

			//std::cout << "Processing: " << fullPath << "\n";
			view->ui.label_intensity_progress_2->setText(QString::fromStdString(fileNameNoExt));
			QApplication::processEvents();


			if (model->Intensity_ptr != nullptr) {
				delete[] model->Intensity_ptr;
				model->Intensity_ptr = nullptr;
			}
			if (model->liv_ptr != nullptr) {
				delete[] model->liv_ptr;
				model->liv_ptr = nullptr;
			}
			if (model->octa_result_ptr != nullptr) {

				delete[] model->octa_result_ptr;
				model->octa_result_ptr = nullptr;
			}
			if (model->oct_intensity_mask != nullptr) {
				delete[] model->oct_intensity_mask;
				model->oct_intensity_mask = nullptr;
			}


			model->loadAndProcessOCTRawData(QString::fromStdString(fullPath));
			model->compute_Intensity_mask(view->ui.global_threshold_2->value());
			model->octa_result_ptr = model->compute_OCTA(view->ui.spinBox_gm->value(), view->ui.spinBox_gn->value(), view->ui.spinBox_gi->value());


			// save the file 
			QDir().mkpath(QString::fromStdString(octa_outputFolder));
			QDir().mkpath(QString::fromStdString(oct_intensityFolder));

			// save the file
			QString octadataPath = QString::fromStdString(octa_outputFolder) + "/blood.png";
			QString intensitypngPath = QString::fromStdString(oct_intensityFolder) + "/intensity.png";

			saveoctaData(octadataPath);
			//saveoctIntensityPng(intensitypngPath);

		}
	}

}


void Controller::saveoctaData(QString filename) {


	if (!filename.isEmpty()) {
		if (filename.endsWith(".raw")) {

			float* octa_masked = new float[model->Z_pixel * model->X_pixel * model->Y_pixel];

			for (int i = 0; i < model->Z_pixel * model->X_pixel * model->Y_pixel; i++) {
				if (model->oct_intensity_mask[i] > 0) {
					octa_masked[i] = model->octa_result_ptr[i];
				}
				else {
					octa_masked[i] = 0;
				}
			}

			saveRaw(filename.toStdString(), octa_masked, model->Z_pixel * model->X_pixel * model->Y_pixel);
			delete[] octa_masked;
		}

		if (filename.endsWith(".png")) {
			filename.chop(4);

			float* octa_masked = new float[model->Z_pixel * model->X_pixel * model->Y_pixel];

			for (int i = 0; i < model->Z_pixel * model->X_pixel * model->Y_pixel; i++) {
				if (model->oct_intensity_mask[i] > 0) {
					octa_masked[i] = model->octa_result_ptr[i];
				}
				else {
					octa_masked[i] = 0;
				}
			}



			saveOCTSlicesAsPNG(octa_masked, model->Z_pixel, model->X_pixel, model->Y_pixel, filename.toStdString());
			delete[] octa_masked;
		}

	}
}


void Controller::saveoctIntensityPng(QString filename) {
	if (filename.endsWith(".png")) {
		filename.chop(4);

		float* oct_intensity_ptr = new float[model->Z_pixel * model->X_pixel * model->Y_pixel];


		for (int i = 0; i < model->Z_pixel * model->X_pixel * model->Y_pixel; i++) {
			float magnitude = std::abs(model->complex_data[i]);
			oct_intensity_ptr[i] = float(20.0 * std::log10(magnitude + 10e-6f));
		}


		saveOCTSlicesAsPNG(oct_intensity_ptr, model->Z_pixel, model->X_pixel, model->Y_pixel, filename.toStdString());
		delete[] oct_intensity_ptr;
	}
}


/**
 * @brief Controller::onLoadIntensityClicked
 *
 * 当用户点击“加载强度数据”按钮时调用该函数。
 * 功能包括弹出文件选择对话框、释放旧资源、加载新的强度图像数据，
 * 更新界面信息并显示当前Y截面对应的强度图。
 * 同时会在操作完成后提示用户加载成功的信息。
 *
 * 注意：此函数不接受参数，也不返回任何值。
 */
void Controller::onLoadIntensityClicked()
{
	
	// 弹出文件选择对话框，获取用户选择的文件路径
	QString fileName = QFileDialog::getOpenFileName(nullptr,
		"请选择OCT文件", "", "OCT文件 (*.oct *.png *.bmp)");

	if (!fileName.isEmpty()) {
		this->setUIStateForIntensityMode();
		if (model->Intensity_ptr != nullptr) {
			delete[] model->Intensity_ptr;  // 清理旧数据
			model->Intensity_ptr = nullptr;
		}
		if (model->octa_result_ptr != nullptr) {
			delete[] model->octa_result_ptr;
			model->octa_result_ptr = nullptr;
		}
		if (model->oct_intensity_mask != nullptr) {
			delete[] model->oct_intensity_mask;
			model->oct_intensity_mask = nullptr;
		}
		// 加载强度数据
		model->getIntensityImage(fileName);

		// 设置滑块范围基于实际的Y截面数量
		int maxYIndex = model->Y_pixel - 1;
		cout << maxYIndex << endl;
		if (maxYIndex < 0) maxYIndex = 0;

		// onLoadClicked() 中的信息显示（更详细）
		view->ui.label_scanMode->setText(QString::fromStdString(model->Scan_mode));
		view->ui.label_fov->setText(
			QString::number(model->Z_range) + "mm" + " x " +
			QString::number(model->X_range) + "mm" + " x " +
			QString::number(model->Y_range) + "mm"
		);
		view->ui.label_imageSize->setText(
			QString::number(model->Z_pixel) + "piexl" + " x " +
			QString::number(model->X_pixel) + "piexl" + " x " +
			QString::number(model->Y_pixel) + "piexl"
		);

		view->ui.intensityDataSlider->setMinimum(0);
		view->ui.intensityDataSlider->setMaximum(maxYIndex);

		// 在imageLabel上显示当前切片的强度图像
		ws->displayIntensityImage(model, view, view->ui.intensityDataSlider->value());

		// 显示成功消息
		QMessageBox::information(nullptr, "加载成功",
			QString("强度数据加载成功！\n文件: %1\n尺寸: %2×%3×%4")
			.arg(fileName)
			.arg(model->X_pixel)
			.arg(model->Y_pixel)
			.arg(model->Z_pixel));
	}
	// 保存各个y截面强度数据为png格式


}


/**
 * @brief 处理强度滑块值变化事件
 *
 * 当用户调整强度滑块时，此函数会被调用以响应滑块值的变化。
 * 该函数通常用于更新相关的显示或控制逻辑。
 *
 * @param value 滑块的新值，表示当前的强度等级
 */
void Controller::IntensitySliderValueChanged(int value)
{
	ws->displayIntensityImage(model, view, value);
}


cv::Mat Controller::generateIntensitySliceImage(int sliceIndex)
{
	// 安全检查
	if (model->Intensity_ptr == nullptr) {
		qDebug() << "错误：强度数据未加载";
		return cv::Mat();
	}

	// 检查切片索引有效性
	if (sliceIndex < 0 || sliceIndex >= model->Y_pixel) {
		qDebug() << "错误：切片索引无效:" << sliceIndex;
		return cv::Mat();
	}

	const int frameSize = model->X_pixel * model->Z_pixel;
	const int dataOffset = sliceIndex * frameSize;

	// 安全检查数据范围
	if (dataOffset + frameSize > model->Y_pixel * frameSize) {
		qDebug() << "错误：数据索引超出范围";
		return cv::Mat();
	}

	// 创建OpenCV矩阵存储强度数据
	cv::Mat intensityMat(model->X_pixel, model->Z_pixel, CV_32FC1,
		model->Intensity_ptr + dataOffset);

	// 归一化到0-255范围
	cv::Mat normalizedMat;
	cv::normalize(intensityMat, normalizedMat, 0, 255, cv::NORM_MINMAX, CV_8UC1);

	// 转置图像以适应显示方向
	cv::Mat transposedMat;
	cv::transpose(normalizedMat, transposedMat);

	return transposedMat;
}


/**
 * @brief 批量保存所有Y截面的强度数据为PNG格式
 * @param outputDir 输出目录路径
 *
 * 该函数遍历所有Y截面，将每个截面的强度数据保存为单独的PNG图像文件，
 * 并生成统一的元数据文件，便于批量处理和分析。
 */
void Controller::batchSaveIntensityAsPng(const QString& outputDir)
{
	// 检查强度数据是否可用
	if (model->Intensity_ptr == nullptr) {
		QMessageBox::warning(nullptr, "批量保存失败",
			"没有可用的强度数据，请先加载OCT文件。");
		return;
	}

	// 创建输出目录
	QDir dir(outputDir);
	if (!dir.exists()) {
		if (!dir.mkpath(".")) {
			QMessageBox::critical(nullptr, "批量保存失败",
				QString("无法创建输出目录:\n%1\n请检查写入权限。").arg(outputDir));
			return;
		}
	}

	// 检查目录是否可写
	if (!QFileInfo(outputDir).isWritable()) {
		QMessageBox::critical(nullptr, "批量保存失败",
			QString("输出目录不可写:\n%1\n请检查写入权限。").arg(outputDir));
		return;
	}

	int successCount = 0;
	int totalSlices = model->Y_pixel;

	// 创建进度对话框
	QProgressDialog progress("批量保存强度数据...", "取消", 0, totalSlices, nullptr);
	progress.setWindowModality(Qt::WindowModal);
	progress.setMinimumDuration(0);
	progress.setValue(0);

	// 记录开始时间
	QDateTime startTime = QDateTime::currentDateTime();

	qDebug() << "开始批量保存强度数据，总切片数:" << totalSlices;

	// 遍历所有Y截面
	for (int yIndex = 0; yIndex < totalSlices; ++yIndex) {
		// 检查用户是否取消操作
		if (progress.wasCanceled()) {
			break;
		}

		// 更新进度
		progress.setValue(yIndex);
		progress.setLabelText(QString("正在处理切片 %1/%2").arg(yIndex + 1).arg(totalSlices));

		// 生成当前切片的图像
		cv::Mat intensityImage = generateIntensitySliceImage(yIndex);
		if (intensityImage.empty()) {
			qDebug() << "警告：无法生成切片" << yIndex << "的图像";
			continue;
		}

		// 生成文件名（使用4位数字填充，便于排序）
		QString fileName = QString("%1/Intensity_Slice_%2.png")
			.arg(outputDir).arg(yIndex, 4, 10, QChar('0'));

		// 设置PNG压缩参数
		std::vector<int> compression_params;
		compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
		compression_params.push_back(6); // 中等压缩比，平衡文件大小和保存速度
		compression_params.push_back(cv::IMWRITE_PNG_STRATEGY);
		compression_params.push_back(cv::IMWRITE_PNG_STRATEGY_DEFAULT);

		// 保存为PNG格式
		bool saveSuccess = cv::imwrite(fileName.toStdString(), intensityImage, compression_params);

		if (saveSuccess) {
			successCount++;
			qDebug() << "成功保存切片:" << fileName;
		}
		else {
			qDebug() << "保存失败切片:" << fileName;
		}

		// 处理事件循环，保持UI响应
		QApplication::processEvents();
	}

	progress.setValue(totalSlices);

	// 计算处理时间
	QDateTime endTime = QDateTime::currentDateTime();
	qint64 elapsedSeconds = startTime.secsTo(endTime);

	// 生成批量处理的元数据文件
	QString metaFileName = QString("%1/Batch_Processing_Metadata.txt").arg(outputDir);
	QFile metaFile(metaFileName);
	if (metaFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream out(&metaFile);
		out << "强度数据批量处理元数据\n";
		out << "========================\n";
		out << "处理时间: " << startTime.toString("yyyy-MM-dd hh:mm:ss") << "\n";
		out << "完成时间: " << endTime.toString("yyyy-MM-dd hh:mm:ss") << "\n";
		out << "处理耗时: " << elapsedSeconds << " 秒\n";
		out << "总切片数: " << totalSlices << "\n";
		out << "成功保存: " << successCount << "\n";
		out << "失败数量: " << (totalSlices - successCount) << "\n";
		out << "输出目录: " << outputDir << "\n";
		out << "原始数据尺寸: " << model->X_pixel << " x " << model->Z_pixel << "\n";
		out << "Y方向总层数: " << model->Y_pixel << "\n";
		out << "数据范围: " << model->X_range << " mm x " << model->Z_range << " mm\n";
		out << "文件命名格式: Intensity_Slice_XXXX.png (XXXX为4位切片索引)\n";
		out << "图像格式: PNG灰度图(8位)\n";
		out << "\n文件列表:\n";

		// 列出所有保存的文件
		QStringList nameFilters;
		nameFilters << "Intensity_Slice_*.png";
		QFileInfoList files = dir.entryInfoList(nameFilters, QDir::Files, QDir::Name);
		for (const QFileInfo& fileInfo : files) {
			out << fileInfo.fileName() << "\n";
		}

		metaFile.close();
	}

	// 显示结果摘要
	QString resultMessage = QString(
		"批量保存完成！\n\n"
		"成功保存: %1/%2 个切片\n"
		"处理耗时: %3 秒\n"
		"输出目录: %4\n"
		"元数据文件: %5"
	).arg(successCount).arg(totalSlices).arg(elapsedSeconds).arg(outputDir).arg(metaFileName);

	if (successCount == totalSlices) {
		QMessageBox::information(nullptr, "批量保存完成", resultMessage);
	}
	else {
		QMessageBox::warning(nullptr, "批量保存完成（部分失败）", resultMessage);
	}

	qDebug() << "批量保存完成:" << resultMessage;
}


/**
 * @brief 通过对话框选择目录并执行批量保存
 */
void Controller::onBatchSaveIntensityClicked()
{
	// 检查强度数据是否可用
	if (model->Intensity_ptr == nullptr) {
		QMessageBox::warning(nullptr, "批量保存失败",
			"没有可用的强度数据，请先加载OCT文件。");
		return;
	}

	// 弹出目录选择对话框
	QString outputDir = QFileDialog::getExistingDirectory(
		nullptr,
		"选择批量保存的输出目录",
		QDir::homePath(),
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	);

	if (!outputDir.isEmpty()) {
		// 执行批量保存
		batchSaveIntensityAsPng(outputDir);
	}
}


/**
 * @brief 设置UI界面状态为强度模式
 *
 * 该函数用于在切换到强度数据处理模式时，更新用户界面控件的启用/禁用状态。
 * 它会禁用与RawData相关的控件，并启用与强度数据相关的控件。
 *
 * @param 无参数
 * @return 无返回值
 */
void Controller::setUIStateForIntensityMode()
{
	// 禁用RawData相关控件
    view->ui.button_save_structure->setEnabled(false);
    view->ui.button_saveocta_data->setEnabled(false);
	view->ui.horizontalSlider->setEnabled(false);
	view->ui.OCTA_slider->setEnabled(false);
	view->ui.slider_intensity_mask->setEnabled(false);
	view->ui.OCTAButton->setEnabled(false);
	view->ui.pushButton_show_mask->setEnabled(false);

	// 启用强度数据相关控件
	view->ui.intensityDataSlider->setEnabled(true);
	view->ui.saveIntensityButton->setEnabled(true);
}


/**
 * @brief 设置UI界面状态为原始数据模式
 *
 * 该函数用于将用户界面控件的状态切换到原始数据模式，
 * 启用与原始数据处理相关的控件，同时禁用与强度数据处理相关的控件。
 *
 * @note 该函数无参数且无返回值
 */
void Controller::setUIStateForRawDataMode()
{
	// 启用RawData相关控件
	view->ui.button_save_structure->setEnabled(true);
	view->ui.button_saveocta_data->setEnabled(true);
	view->ui.horizontalSlider->setEnabled(true);
	view->ui.OCTA_slider->setEnabled(true);
	view->ui.slider_intensity_mask->setEnabled(true);
	view->ui.OCTAButton->setEnabled(true);
	view->ui.pushButton_show_mask->setEnabled(true);

	// 禁用强度数据相关控件
	view->ui.intensityDataSlider->setEnabled(false);
	view->ui.saveIntensityButton->setEnabled(false);
}

