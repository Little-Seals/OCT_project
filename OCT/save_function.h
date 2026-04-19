#pragma once
#include <iostream>
#include <string>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include "SpectralRadar.h"  


void saveOCTintensitySlicesAsPNG(float* data, int Z, int X, int Y, const std::string& outputFolder);
void processOCTFile(const std::string& octPath, const std::string& outputFolder);