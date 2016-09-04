#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkPasteImageFilter.h"

#include <iostream>
#include <string>
#include <sstream>

typedef itk::Image<unsigned char, 2>     ImageType;
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

  ImageType::Pointer image1 = ImageType::New();
  CreateImage1(image1, height, width);

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName("fruit1_1_6.png");
  reader->Update();

  ImageType::Pointer image2 = reader->GetOutput();

  typedef itk::PasteImageFilter <ImageType, ImageType >
    PasteImageFilterType;
  typedef itk::ImageFileWriter< ImageType > WriterType;


  ImageType::IndexType destinationIndex;
  destinationIndex[0] = 0;
  destinationIndex[1] = 0;

  PasteImageFilterType::Pointer pasteFilter
    = PasteImageFilterType::New ();
  pasteFilter->SetSourceImage(image2);
  pasteFilter->SetSourceRegion(image2->GetLargestPossibleRegion());

  pasteFilter->SetDestinationImage(image1);
  pasteFilter->SetDestinationIndex(destinationIndex);

  WriterType::Pointer writer = WriterType::New();

  std::string name = "fullImage";
  std::string format = "png";
  std::stringstream ss2;
  std::string pathToFolder = ".";
  if(pathToFolder[pathToFolder.size()-1] != '/')
    pathToFolder += '/';
  ss2 << pathToFolder << name << "." << format;
  std::string filename_out;
  ss2 >> filename_out;
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



void CreateImage2(ImageType::Pointer image)
{
  // Create an image
  ImageType::RegionType region;
  ImageType::IndexType start;
  start[0] = 0;
  start[1] = 0;

  ImageType::SizeType size;
  unsigned int NumRows = 400;
  unsigned int NumCols = 500;
  size[0] = NumRows;
  size[1] = NumCols;

  region.SetSize(size);
  region.SetIndex(start);

  image->SetRegions(region);
  image->Allocate();

  // Make another square
  for(unsigned int r = 0; r < NumRows; r++)
    {
    for(unsigned int c = 0; c < NumCols; c++)
      {
      ImageType::IndexType pixelIndex;
      pixelIndex[0] = r;
      pixelIndex[1] = c;

      image->SetPixel(pixelIndex, 200);
      }
    }
}

/*
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkRGBPixel.h"

#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

int main(int argc, char *argv[])
{
  if(argc < 2)
    {
    std::cerr << "Usage: ";
    std::cerr << argv[0] << " inputImageFile pathToFolder [rows] [cols]" << std::endl;
    return EXIT_FAILURE;
    }

  std::string filename = argv[1];

  typedef itk::Image<itk::RGBPixel<unsigned char>, 2> ImageType;
  typedef itk::ImageFileReader<ImageType>             ReaderType;
  typedef itk::ImageFileWriter< ImageType >           WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(filename.c_str());
  reader->Update();

  ImageType::SizeType inSize = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
  std::cout << "size " << inSize[0] << " " << inSize[1] << std::endl;
  typedef itk::RegionOfInterestImageFilter< ImageType, ImageType > ROIFilterType;
  ROIFilterType::Pointer roiFilter = ROIFilterType::New();

  int rows = 10;
  int cols = 10;
  if (argc < 3) {
    std::cerr << "too few arguments" << std::endl;
    return EXIT_FAILURE;
  }
  std::string pathToFolder(argv[2]);

  if (argc > 3) {
    rows = atoi(argv[3]);
  }
  if (argc > 4) {
    cols = atoi(argv[4]);
  }

  ImageType::IndexType start;
  ImageType::SizeType size;
  int pixel_x = 0;
  int pixel_y = 0;
  int jump_x = inSize[0] / cols;
  int jump_y = inSize[1] / rows;
  for(int i = 0; i < cols; i++) {
    start[0] = pixel_x;
    pixel_x += jump_x;
    size[0] = std::min(jump_x, (int)inSize[0]-(int)start[0]);
    pixel_y = 0;
    for(int j = 0; j < rows; j++) {
      start[1] = pixel_y;
      pixel_y += jump_y;
      size[1] = std::min(jump_y, (int)inSize[1]-(int)start[1]);

      ImageType::RegionType desiredRegion;
      desiredRegion.SetSize(size);
      desiredRegion.SetIndex(start);

      roiFilter->SetRegionOfInterest(desiredRegion);
      roiFilter->SetInput(reader->GetOutput());

      std::cout << "RegionOfInterestImageFilter\nstart = " << start
           << " size = " << size << std::endl;

      WriterType::Pointer writer = WriterType::New();

      std::string name;
      std::stringstream ss(filename);
      std::getline(ss, name, '.');
      std::cout << "name " << name << std::endl;
      std::string format;
      std::getline(ss, format);

      std::stringstream ss2;
      if(pathToFolder[pathToFolder.size()-1] != '/')
        pathToFolder += '/';
      ss2 << pathToFolder << name << "_" << i << "_" << j << "." << format;
      std::string filename_out;
      ss2 >> filename_out;
      std::cout << "output file: " << filename_out << std::endl;
      writer->SetFileName( filename_out.c_str() );
      writer->SetInput( roiFilter->GetOutput() );
      try {
         writer->Update();
      } catch( itk::ExceptionObject & error ) {
         std::cerr << "Error: " << error << std::endl;
         return EXIT_FAILURE;
      }
    }
  }
  return EXIT_SUCCESS;
}

*/
