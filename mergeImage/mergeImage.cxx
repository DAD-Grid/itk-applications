#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkPasteImageFilter.h"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <fstream>

typedef itk::Image<itk::RGBPixel<unsigned char>, 2>   ImageType;
typedef itk::ImageFileReader<ImageType>               ReaderType;
typedef itk::PasteImageFilter <ImageType, ImageType > PasteImageFilterType;
typedef itk::ImageFileWriter< ImageType >             WriterType;

using namespace std;

static void CreateDestImage(ImageType::Pointer image, unsigned int NumRows, unsigned int NumCols)
{
  // Create an image with 2 connected components
  ImageType::RegionType region;
  ImageType::IndexType start;
  start[0] = 0;
  start[1] = 0;

  ImageType::SizeType size;
  size[0] = NumRows;
  size[1] = NumCols;
  std::cout << "size " << size[0] << " " << size[1] << std::endl;
  region.SetSize(size);
  region.SetIndex(start);
  image->SetRegions(region);
  image->Allocate();
}

int main(int argc, char *argv[])
{
  if(argc < 6) {
    std::cerr << "Usage: ";
    std::cerr << argv[0] << " height width numRows numCols folderPath" << std::endl;
    return EXIT_FAILURE;
  }

  int height = atoi(argv[1]);
  int width = atoi(argv[2]);
  int numRows = atoi(argv[3]);
  int numCols = atoi(argv[4]);
  std::string folderPath = argv[5];

  ImageType::Pointer destImg = ImageType::New();
  CreateDestImage(destImg, height, width);
 
  
  ImageType::SizeType destSize = destImg->GetLargestPossibleRegion().GetSize();

  std::string comando = "ls " + folderPath + " > nombresArchivos.txt";
 std::cout << comando << std::endl; 
  system(comando.c_str());

  std::ifstream in("nombresArchivos.txt");
  std::string fileName;
  while(in >> fileName) {
    std::cout << fileName << std::endl;
    //get src position in destination
    std::string namef;
    std::stringstream ss(fileName);
    std::getline(ss, namef, '.');
    std::string formatF;
    std::getline(ss, formatF);
    
    std::stringstream ss2(namef);
    std::string str;
    std::vector<std::string> nameArr;
    while(std::getline(ss2, str, '_') != NULL) {
      std::cout << str << std::endl;
      nameArr.push_back(str);
    }

    int row = atoi(nameArr[1].c_str());
    int col = atoi(nameArr[2].c_str());

    int singleImageHeight =  (int)(destSize[0]) / numRows;
    int singleImageWith = (int)(destSize[1]) / numCols;
    ImageType::IndexType destinationIndex;
    destinationIndex[0] = row*singleImageHeight;
    destinationIndex[1] = col*singleImageWith;

    std::cout<< "index " << destinationIndex[0] << " " << destinationIndex[1] << std::endl;


    ReaderType::Pointer reader2 = ReaderType::New();
    reader2->SetFileName(folderPath + fileName);
    reader2->Update();

    ImageType::Pointer srcImage = reader2->GetOutput();
    
    PasteImageFilterType::Pointer pasteFilter = PasteImageFilterType::New ();
    pasteFilter->SetDestinationImage(destImg);
    pasteFilter->SetSourceImage(srcImage);
    pasteFilter->SetSourceRegion(srcImage->GetLargestPossibleRegion());
    pasteFilter->SetDestinationIndex(destinationIndex);
    pasteFilter->Update();
    destImg = pasteFilter->GetOutput();

  }

  WriterType::Pointer writer = WriterType::New();

  std::string name = "destImage";
  std::string format = "png";
  std::stringstream ss3;
  std::string pathToFolder = ".";
  if(pathToFolder[pathToFolder.size()-1] != '/')
    pathToFolder += '/';
  ss3 << pathToFolder << name << "." << format;
  std::string filename_out;
  ss3 >> filename_out;
  std::cout << "output file: " << filename_out << std::endl;
  writer->SetFileName( filename_out.c_str() );
  writer->SetInput( destImg );
  try {
     writer->Update();
  } catch( itk::ExceptionObject & error ) {
     std::cerr << "Error: " << error << std::endl;
     return EXIT_FAILURE;
  }

  system("rm -rf nombresArchivos.txt");

  return EXIT_SUCCESS;
}