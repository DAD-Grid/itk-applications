#include "itkRGBPixel.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkPasteImageFilter.h"
#include "itkVectorCastImageFilter.h"
#include <itkExtractImageFilter.h>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <fstream>

typedef itk::RGBPixel< unsigned char >                PixelType;
typedef itk::Image< PixelType, 2 >                    InputImageType;
typedef itk::ImageFileReader<InputImageType>               ReaderType;
typedef itk::PasteImageFilter <InputImageType, InputImageType > PasteImageFilterType;

typedef itk::RGBPixel< unsigned char >                WritePixelType;
typedef itk::Image< WritePixelType, 2 >               WriteImageType;
typedef itk::ImageFileWriter< WriteImageType >        WriterType;
typedef itk::VectorCastImageFilter<
                    InputImageType, WriteImageType >  CasterType;
typedef itk::ExtractImageFilter< InputImageType, InputImageType > ExactFilterType;

using namespace std;

static void CreateDestImage(InputImageType::Pointer image, unsigned int NumRows, unsigned int NumCols)
{
  InputImageType::RegionType region;
  InputImageType::IndexType start;
  start[0] = 0;
  start[1] = 0;

  InputImageType::SizeType size;
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
  if(argc < 7) {
    std::cerr << "Usage: ";
    std::cerr << argv[0] << " height width numRows numCols folderPath delta" << std::endl;
    return EXIT_FAILURE;
  }

  int height = atoi(argv[1]);
  int width = atoi(argv[2]);
  int numRows = atoi(argv[3]);
  int numCols = atoi(argv[4]);
  std::string folderPath = argv[5];
  int delta = atoi(argv[6]);
  if(folderPath[folderPath.size()-1] != '/')
    folderPath += '/';

  InputImageType::Pointer destImg = InputImageType::New();
  CreateDestImage(destImg, height, width);


  InputImageType::SizeType destSize = destImg->GetLargestPossibleRegion().GetSize();

  std::string comando = "ls " + folderPath + " > nombresArchivos.txt";
 std::cout << comando << std::endl;
  system(comando.c_str());

  PasteImageFilterType::Pointer pasteFilter = PasteImageFilterType::New ();
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

    ReaderType::Pointer reader2 = ReaderType::New();
    reader2->SetFileName(folderPath + fileName);
    reader2->Update();

    InputImageType::Pointer srcImage = reader2->GetOutput();
    InputImageType::SizeType srcSize = srcImage->GetLargestPossibleRegion().GetSize();

    InputImageType::IndexType cropStart;
    
    if(row == 0) cropStart[0] = 0;
    else         cropStart[0] = delta; 

    if(col == 0) cropStart[1] = 0;
    else         cropStart[1] = delta;
  
    InputImageType::SizeType cropSize;
    
    if(row == numRows-1 || row == 0) cropSize[0] = srcSize[0] - delta;
    else                             cropSize[0] = srcSize[0] - (2*delta);

    if(col == numCols-1 || col == 0) cropSize[1] = srcSize[1] - delta;
    else                             cropSize[1] = srcSize[1] - (2*delta);

    InputImageType::RegionType cropRegion(cropStart, cropSize);
    
    ExactFilterType::Pointer exactfilter = ExactFilterType::New();
    exactfilter->SetExtractionRegion(cropRegion);
    exactfilter->SetInput(srcImage);
    // exactfilter->SetDirectionCollapseToIdentity(); // This is required.
    exactfilter->Update();    

    int singleImageHeight =  (int)(destSize[0]) / numRows;
    int singleImageWith = (int)(destSize[1]) / numCols;
    InputImageType::IndexType destinationIndex;
    destinationIndex[0] = row*singleImageHeight;
    destinationIndex[1] = col*singleImageWith;
    std::cout << "----------------delta: " << delta << std::endl;
    // if (destinationIndex[0] != 0) destinationIndex[0] -= delta;
    // if (destinationIndex[1] != 0) destinationIndex[1] -= delta;

    std::cout<< "index " << destinationIndex[0] << " " << destinationIndex[1] << std::endl;
    std::cout << "size " << exactfilter->GetOutput()->GetLargestPossibleRegion() << std::endl;
    pasteFilter->SetDestinationImage(destImg);
    pasteFilter->SetSourceImage(exactfilter->GetOutput());
    pasteFilter->SetSourceRegion(exactfilter->GetOutput()->GetLargestPossibleRegion());
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
