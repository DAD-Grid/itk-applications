#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkPasteImageFilter.h"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

typedef itk::Image<itk::RGBPixel<unsigned char>, 2> ImageType;
typedef itk::ImageFileReader<ImageType>  ReaderType;

int main(int argc, char *argv[])
{
  if(argc < 4) {
    std::cerr << "Usage: ";
    std::cerr << argv[0] << " srcImage numRows numCols" << std::endl;
    return EXIT_FAILURE;
  }
  std::string fileName = argv[1];
  int numRows = atoi(argv[2]);
  int numCols = atoi(argv[3]);
  std::cout<< "filename: " << fileName << " " << numRows << " " << numCols << std::endl;
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

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName("destImage.png");
  reader->Update();

  ImageType::Pointer destImage = reader->GetOutput();

  ImageType::SizeType destSize = destImage->GetLargestPossibleRegion().GetSize();
  std::cout<< "index " << destSize[0] << " " << destSize[1] << std::endl;
  int singleImageHeight =  (int)(destSize[0]) / numRows;
  int singleImageWith = (int)(destSize[1]) / numCols;
  ImageType::IndexType destinationIndex;
  destinationIndex[0] = row*singleImageHeight;
  destinationIndex[1] = col*singleImageWith;

  std::cout<< "index " << destinationIndex[0] << " " << destinationIndex[1] << std::endl;


  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName(fileName);
  reader2->Update();

  ImageType::Pointer srcImage = reader2->GetOutput();

  typedef itk::PasteImageFilter <ImageType, ImageType >
    PasteImageFilterType;
  typedef itk::ImageFileWriter< ImageType > WriterType;


  PasteImageFilterType::Pointer pasteFilter
    = PasteImageFilterType::New ();
  pasteFilter->SetSourceImage(srcImage);
  pasteFilter->SetSourceRegion(srcImage->GetLargestPossibleRegion());

  pasteFilter->SetDestinationImage(destImage);
  pasteFilter->SetDestinationIndex(destinationIndex);

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
  writer->SetInput( pasteFilter->GetOutput() );
  try {
     writer->Update();
  } catch( itk::ExceptionObject & error ) {
     std::cerr << "Error: " << error << std::endl;
     return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}