#include "main.h"

// This function checks if the file has extension .ms
bool check_extension(std::string filename){
  if(!filename.compare(filename.length()-3, 3, ".ms"))
    return true;
  return false;
}

int main(int argc, char **argv)
{
  if(argc == 1)
    printf("[ERROR] The path for the file is required\n");
  std::string filename(argv[1]);
  std::fstream file(argv[1], std::fstream::in);
  std::string text;
  std::string output;
  Parser parser;
  MichaelScott *code;
  output = "output.o";
  if(argc >= 3)
    output = argv[2];

  printf("[INFO] Getting the %s\n", argv[1]);
  // If occurred a error
  if(file.fail()){
    printf("[ERROR] An error has occurred\n");
  }
  // If the extension is wrong
  else if(!check_extension(filename)){
    printf("[ERROR] The extension must be .ms\n");
  }
  // If is everything alright, we parse the file
  else if(file.good()){
    std::getline(file, text, '\0');
    file.close();
    code = parser.parse(text.c_str());
    code->codegen();
    CreateObjectFile(output);
  }
  // If it is not alright
  else
    printf("[ERROR] An error has occurred\n");
  printf("[INFO] Code generated: %s\n", output.c_str());
  return 0;
}