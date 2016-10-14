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
  if(argc < 3)
  {
    std::cerr << "Usage: ";
    std::cerr << argv[0] << " inputImageFile pathToFolder [rows] [cols] [delta]" << std::endl;
    return EXIT_FAILURE;
  }

  std::string filename = argv[1];
  std::string pathToFolder(argv[2]);
  int rows = 2;
  int cols = 2;
  int delta = 0;
  if (argc > 3) {
    rows = atoi(argv[3]);
  }
  if (argc > 4) {
    cols = atoi(argv[4]);
  }
  if (argc > 5) {
    delta = atoi(argv[5]);
  }

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

  
  if (argc < 3) {
    std::cerr << "too few arguments" << std::endl;
    return EXIT_FAILURE;
  }
  

  ImageType::IndexType start;
  ImageType::SizeType size;
  int pixel_x = 0;
  int pixel_y = 0;
  int jump_x = inSize[0] / cols;
  int jump_y = inSize[1] / rows;
  for(int i = 0; i < cols; i++) {
    int sizex;
    if(i == 0) {
        start[0] = pixel_x;
      sizex = jump_x + delta;
      } else if(i == cols-1) {
        start[0] = pixel_x - delta;
        sizex = jump_x + delta;
      } else {
        start[0] = pixel_x - delta;
        sizex = jump_x + (2*delta);
      }

    std::cout << "startx " << start[0] << " end " << start[0] + sizex << std::endl;
    std::cout << "sizex " << sizex << std::endl;
    std::cout << std::endl;
    pixel_x += jump_x;
    
    size[0] = std::min(sizex, (int)inSize[0]-(int)start[0]);
    pixel_y = 0;
    for(int j = 0; j < rows; j++) {
      int sizey;
      if(j == 0) {
        start[1] = pixel_y;
        sizey = jump_y + delta;
      } else if(j == rows-1) {
        start[1] = pixel_y - delta;
        sizey = jump_y + delta;
      } else {
        start[1] = pixel_y - delta;
        sizey = jump_y + (2*delta);
      }
      std::cout << "starty " << start[1] << " end " << start[1] + sizey << std::endl;
      std::cout << "sizey " << sizey << std::endl;
      std::cout << std::endl;
      pixel_y += jump_y;
      size[1] = std::min(sizey, (int)inSize[1]-(int)start[1]);

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
      std::string onlyName = name;
      while(std::getline(ss, onlyName, '/') != NULL) {
      }
      std::stringstream sss(onlyName);
      std::cout << "onlyName " << onlyName << std::endl;
      std::getline(sss, name, '.');
      std::cout << "name " << name << std::endl;
      std::string format;
      std::getline(sss, format);

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
