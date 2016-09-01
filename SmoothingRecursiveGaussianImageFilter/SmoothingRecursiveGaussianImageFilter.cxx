#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkRGBPixel.h"

#include <string>

int main(int argc, char * argv[])
{
  // Verify command line arguments
  // if( argc < 2 )
  // {
  //   std::cerr << "Usage: " << std::endl;
  //   std::cerr << argv[0] << " inputImageFile [sigma]" << std::endl;
  //   return EXIT_FAILURE;
  // }

  double sigma = 4.0;
  // if (argc > 2)
  // {
  //   sigma = atof(argv[2]);
  // }

  // std::string filename = argv[1];
  std::string filename = "fruit1.png";

  const unsigned int Dimension = 2;
  typedef unsigned char PixelComponentType;

  typedef itk::Image<itk::RGBPixel< PixelComponentType>,
    Dimension > ColorImageType;

  typedef itk::ImageFileReader< ColorImageType >
    ReaderType;
  // Create and setup a reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( filename );
  reader->Update();

  typedef itk::SmoothingRecursiveGaussianImageFilter<
    ColorImageType, ColorImageType >  FilterType;

  FilterType::Pointer smoothingRecursiveGaussianImageFilter = FilterType::New();
  smoothingRecursiveGaussianImageFilter->SetInput(reader->GetOutput());
  smoothingRecursiveGaussianImageFilter->SetSigma(sigma);
  smoothingRecursiveGaussianImageFilter->Update();

  typedef itk::ImageFileWriter< ColorImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();

  std::string name;
  std::stringstream ss(filename);
  std::getline(ss, name, '.');

  std::string format;
  std::getline(ss, format);

  filename = name + "_salida."+format;
  std::cout << "output file: " << filename << std::endl;
  writer->SetFileName( filename.c_str() );
  writer->SetInput( smoothingRecursiveGaussianImageFilter->GetOutput() );
  try {
      writer->Update();
  } catch( itk::ExceptionObject & error ) {
      std::cerr << "Error: " << error << std::endl;
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
