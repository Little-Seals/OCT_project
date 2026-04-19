//#include<SpectralRadar.h>
//
//int main() {
//
//}
//
//void ReadOCTFile()
//{
//	// This example program shows how to read an oct-file with the SDK which has been acquired and saved with ThorImageOCT.
//	// To make sure the correct parameters will be used to modify the loaded data, e.g. the files for the processing from the dataset and not the current ones for an acquisition, 
//	// it is necessary to use the functions specified for an OCTFile as in this example.
//	OCTFileHandle OCTFile = createOCTFile(FileFormat_OCITY);
//
//	// Please select an .oct-file you want to load
//	WriteOCTFile(); // this calls the demo program from above and creates an .oct-file
//	loadFile(OCTFile, "AcquiredOCTFileWithSDK.oct");
//
//	ProbeHandle Probe = initProbeFromOCTFile(nullptr, OCTFile);
//	ProcessingHandle Proc = createProcessingForOCTFile(OCTFile);
//
//	// with the functions handling the metadata the informations from the dataset can be loaded
//	double RangeX = getFileMetadataFloat(OCTFile, FileMetadata_RangeX);
//
//	// load intensity data to DataHandle Intensity from OCTFile
//	DataHandle Intensity = createData();
//	getFileRealData(OCTFile, Intensity, findFileDataObject(OCTFile, DataObjectName_OCTData));
//
//	if (OnError())
//		cout << "ReadOCTFile did not finish successful. " << endl;
//	else
//		cout << "ReadOCTFile finished. " << endl;
//}
//
//bool OnError(void)
//{
//	char errorMessage[1024];
//	if (getError(errorMessage, 1024))
//	{
//		cout << "ERROR: " << errorMessage << endl;
//		_getch();
//		return true;
//	}
//	return false;
//}