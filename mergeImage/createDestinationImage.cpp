#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkPasteImageFilter.h"

#include <iostream>
#include <string>
#include <sstream>

typedef itk::Image<itk::RGBPixel<unsigned char>, 2> ImageType;
typedef itk::ImageFileReader<ImageType>  ReaderType;


static void CreateImage1(ImageType::Pointer image, unsigned int NumRows, unsigned int NumCols)
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
  int height = 10;
  int width = 10;

  if (argc > 1) {
    height = atoi(argv[1]);
  }
  if (argc > 2) {
    width = atoi(argv[2]);
  }

  ImageType::Pointer destImg = ImageType::New();

  CreateImage1(destImg, height, width);

  typedef itk::ImageFileWriter< ImageType > WriterType;

  WriterType::Pointer writer = WriterType::New();

  std::string name = "destImage.png";
  writer->SetFileName( name.c_str() );
  writer->SetInput( destImg );
  try {
     writer->Update();
  } catch( itk::ExceptionObject & error ) {
     std::cerr << "Error: " << error << std::endl;
     return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}