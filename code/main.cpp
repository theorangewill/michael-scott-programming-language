#include "lexic.h"
#include <iostream>
#include <fstream>
using namespace std;

int main(int argc,char **argv)
{
  fstream file(argv[1], fstream::in);
  string text;
  //Parser parser;

  if(file.good()){
      //Pegar conteudo do arquivo
      getline( file, text, '\0');
      file.close();
      const char * textc = text.c_str();
      cout << text << endl;
      //parser.parse(textc);
  }
  else
    printf("This file does not exist.\n");

  return 0;
}