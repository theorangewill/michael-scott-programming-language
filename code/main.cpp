#include "main.h"

// This function checks if the file has extension .ms
bool check_extension(char *filename_char){
  std::string filename(filename_char);
  if(!filename.compare(filename.length()-3, 3, ".ms"))
    return true;
  return false;
}

int main(int argc,char **argv)
{
  std::fstream file(argv[1], std::fstream::in);
  std::string text;
  Parser parser;
  MichaelScott *code;

  // If occurred a error
  if(file.fail()){
    printf("[ERROR] An error has occurred\n");
  }
  // If the extension is wrong
  else if(!check_extension(argv[1])){
    printf("[ERROR] The extension must be .ms\n");
  }
  // If is everything alright, we parse the file
  else if(file.good()){
    std::getline( file, text, '\0');
    file.close();
    code = parser.parse(text.c_str());
    code->codegen();
  }
  // If it is not alright
  else
    printf("[ERROR] An error has occurred\n");

  return 0;
}