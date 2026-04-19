#include"save_function.h"

void saveOCTintensitySlicesAsPNG(float* data, int Z, int X, int Y, const std::string& outputFolder) {

    float globalMin = data[0];
    float globalMax = data[0];
    int totalSize = Z * X * Y;

    for (int i = 1; i < totalSize; ++i) {
        globalMin = std::min(globalMin, data[i]);
        globalMax = std::max(globalMax, data[i]);
    }

    float range = globalMax - globalMin;
    if (range < 1e-6f) range = 1e-6f;

    for (int y = 0; y < Y; ++y) {
        cv::Mat slice(Z, X, CV_8U);

        for (int x = 0; x < X; ++x) {
            for (int z = 0; z < Z; ++z) {
                int idx = y * Z * X + x * Z + z;
                float val = data[idx];
                int pixel = static_cast<int>(255.0f * (val - globalMin) / range);
                pixel = std::clamp(pixel, 0, 255);
                slice.at<uchar>(z, x) = static_cast<uchar>(pixel);
            }
        }

        std::ostringstream oss;
        oss << outputFolder << "\\" << std::setw(4) << std::setfill('0') << (y + 1) << ".png";
        cv::imwrite(oss.str(), slice);
    }
}


void processOCTFile(const std::string& octPath, const std::string& outputFolder) {
    OCTFileHandle octFile = createOCTFile(FileFormat_OCITY);

    loadFile(octFile, octPath.c_str());

    int X = getFileMetadataInt(octFile, FileMetadata_SizeX);
    int Y = getFileMetadataInt(octFile, FileMetadata_SizeY);
    int Z = getFileMetadataInt(octFile, FileMetadata_SizeZ);

    DataHandle intensity = createData();
    getFileRealData(octFile, intensity, findFileDataObject(octFile, DataObjectName_OCTData));
    float* ptr = getDataPtr(intensity);

    std::filesystem::create_directories(outputFolder);
    saveOCTintensitySlicesAsPNG(ptr, Z, X, Y, outputFolder);

    clearOCTFile(octFile);
    clearData(intensity);
}
