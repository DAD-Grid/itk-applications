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
    std::cerr << argv[0] << " inputImageFile [rows] [cols]" << std::endl;
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
  if (argc > 2) {
    rows = atoi(argv[2]);
  }
  if (argc > 3) {
    cols = atoi(argv[3]);
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
      ss2 << name << "_" << i << "_" << j << "." << format;
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
