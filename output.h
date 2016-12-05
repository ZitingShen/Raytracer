#ifndef OUTPUT_H
#define OUTPUT_H

#include <string>

using namespace std;

typedef enum _output_format{P3, P6} Output_format;

class Output{
  public:
  	Output_format format;
    string file_name;
    int width, height;
}

#endif