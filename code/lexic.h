#ifndef __lexic_h
#define __lexic_h
#ifndef __iostream
    #define __iostream
    #include <iostream>
#endif
#ifndef __cstring
    #define __cstring
    #include <cstring>
#endif
/*
#include <ctype.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
*/

class Token
{
public:
  Token(int type, std::string lexeme):type(type), lexeme(std::move(lexeme)){}
  Token(){};

  void print();

  enum TokenType
  {
    /*ERROR = -1,
    STRING,
    NUMBER,
    TRUE,
    FALSE,
    NULL_,*/
    ERROR = 1,
    EOF_ = 0,
    tok_support = -1,
    tok_statement_support = -2,
    tok_include_support = -3,
    tok_function_support = -4,
    tok_statement = -5,
    tok_symbol = -6,
    tok_main = -7,
    tok_function = -8,
    tok_variable = -9,
    tok_type = -10,
    tok_number = -11,
    tok_mathop = -12,
    tok_compop = -13,
    tok_logicop = -14,
    tok_notop = -15,
    tok_attr = -16,
    tok_string = -17,
    tok_true = -18,
    tok_false = -19
  };

  int type;
  std::string lexeme;

};


static std::string IdentifierStr; // Filled in if tok_identifier
static double NumVal;             // Filled in if tok_number

void Token::print(){
  std::string type_char;
  if(type == Token::ERROR) type_char = "ERROR";
  else if(type == Token::EOF_) type_char = "EOF";
  else if(type == Token::tok_support) type_char = "support";
  else if(type == Token::tok_statement_support) type_char = "statement_support";
  else if(type == Token::tok_include_support) type_char = "include_support";
  else if(type == Token::tok_function_support) type_char = "function_support";
  else if(type == Token::tok_statement) type_char = "statement";
  else if(type == Token::tok_symbol) type_char = "symbol";
  else if(type == Token::tok_main) type_char = "main";
  else if(type == Token::tok_function) type_char = "function";
  else if(type == Token::tok_variable) type_char = "variable";
  else if(type == Token::tok_type) type_char = "type";
  else if(type == Token::tok_number) type_char = "number";
  else if(type == Token::tok_mathop) type_char = "mathop";
  else if(type == Token::tok_compop) type_char = "compop";
  else if(type == Token::tok_logicop) type_char = "logicop";
  else if(type == Token::tok_notop) type_char = "notop";
  else if(type == Token::tok_attr) type_char = "attr";
  else if(type == Token::tok_string) type_char = "string";
  else if(type == Token::tok_true) type_char = "true";
  else if(type == Token::tok_false) type_char = "false";
  printf("\t%s '%s'\n", type_char.c_str(), lexeme.c_str());
}


#endif // __lexic_h