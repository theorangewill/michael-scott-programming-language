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
    tok_symbol = -2,
    tok_main = -3,
    tok_function = -4,
    tok_variable = -5,
    tok_type = -6,
    tok_number = -7,
    tok_mathop = -8,
    tok_comphop = -9,
    tok_logicop = -10,
    tok_attr = -11,
    tok_string = -12,
    tok_true = -13,
    tok_false = -14,
    tok_library = -15
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
  else if(type == Token::tok_symbol) type_char = "symbol";
  else if(type == Token::tok_main) type_char = "main";
  else if(type == Token::tok_function) type_char = "function";
  else if(type == Token::tok_variable) type_char = "variable";
  else if(type == Token::tok_type) type_char = "type";
  else if(type == Token::tok_number) type_char = "number";
  else if(type == Token::tok_mathop) type_char = "mathop";
  else if(type == Token::tok_comphop) type_char = "comphop";
  else if(type == Token::tok_logicop) type_char = "logicop";
  else if(type == Token::tok_attr) type_char = "attr";
  else if(type == Token::tok_string) type_char = "string";
  else if(type == Token::tok_true) type_char = "true";
  else if(type == Token::tok_false) type_char = "false";
  else if(type == Token::tok_library) type_char = "library";
  printf("\t%s '%s'\n", type_char.c_str(), lexeme.c_str());
}


/// gettok - Return the next token from standard input.
static int gettok() {
  static int LastChar = ' ';

  // Skip any whitespace.
  while (isspace(LastChar))
    LastChar = getchar();

  if (isalpha(LastChar)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
    IdentifierStr = LastChar;
    while (isalnum((LastChar = getchar())))
      IdentifierStr += LastChar;

    if (IdentifierStr == "def")
      return Token::tok_support;
    if (IdentifierStr == "extern")
      return Token::tok_symbol;
    return Token::tok_variable;
  }

  if (isdigit(LastChar) || LastChar == '.') { // Number: [0-9.]+
    std::string NumStr;
    do {
      NumStr += LastChar;
      LastChar = getchar();
    } while (isdigit(LastChar) || LastChar == '.');

    NumVal = strtod(NumStr.c_str(), nullptr);
    return Token::tok_number;
  }

  if (LastChar == '#') {
    // Comment until end of line.
    do
      LastChar = getchar();
    while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

    if (LastChar != EOF)
      return gettok();
  }

  // Check for end of file.  Don't eat the EOF.
  if (LastChar == EOF)
    return Token::EOF_;

  // Otherwise, just return the character as its ascii value.
  int ThisChar = LastChar;
  LastChar = getchar();
  return ThisChar;
}




/*
class Token{
    public:
        enum TokenType{
            ERROR = -1,
            EOF_,
            STRING,
            NUMBER,
            TRUE,
            FALSE,
            NULL_
        };

    int type;
    string lexeme;
    char c;
};

class Parser
{
public:
  void parse(const char*);
  int objetos;
  int membros;
  int arrays;
  int elementos;
  Parser();
private:
  char* buffer; // input buffer
  int lineNumber; // current line number
  Token lookahead; // current token

  void error();
  Token nextToken(); // lexical analyser

  void advance()
  {
    lookahead = nextToken();
  }

  void match(int t)
  {
    if (lookahead.type == t)
      advance();
    else
      error();
  }

  void MichaelScott();
  void Components();
  void Include();
  void Declaration();
  void Function();
  void Scranton();
  void Value();
  void Object();
  void MembersOpt();
  void Members();
  void M_();
  void Pair();
  void Array();
  void ElementsOpt();
  void Elements();
  void E_();

}; // Parser

//Construtor do Parser, inicializa com todos os valores em zero
Parser::Parser()
{
  objetos = 0;
  membros = 0;
  arrays = 0;
  elementos = 0;
}

void
Parser::parse(const char* input)
{
  buffer = const_cast<char*>(input);
  lineNumber = 1;
  lookahead = nextToken();
  MichaelScott();
  if (lookahead.type != Token::EOF_)
    error();
  printf("Numero de Objetos: %d\nNumero de Membros: %d\nNumero de Arrays: %d\nNumero de Elementos: %d\n", objetos, membros, arrays, elementos);
  puts("Concluido");
}

void
Parser::error()
{
  printf("**Error (%d): %s\n", lineNumber, "desconhecido");
  exit(1);
}

Token
Parser::nextToken()
{
  while (isspace(*buffer))
    if (*buffer++ == '\n')
      ++lineNumber;

  Token t;
  if (*buffer == '\0')
  {
        t.type = Token::EOF_;
        return t;
  }

  char* beginLexeme = buffer;

  if (*buffer == '-' || isdigit(*buffer))
  {
    if(*buffer == '-'){
      ++buffer;
      if(isdigit(*buffer))
        ++buffer;
    }
    else
        ++buffer;

    while(isdigit(*buffer))
      ++buffer;

    if(*buffer == '.'){
      if(isdigit(*buffer))
        ++buffer;
      while (isdigit(*buffer))
        ++buffer;
    }

    if(*buffer == 'e' || *buffer == 'E'){
      if(*buffer == '+' || *buffer == '-')
      ++buffer;
      if(isdigit(*buffer))
        ++buffer;
      while (isdigit(*buffer))
        ++buffer;
    }

    t.type = Token::NUMBER;
    t.lexeme = string(beginLexeme, buffer);
    return t;
  }

  if (*buffer == 34)
  {
    ++buffer;
    while (*buffer != 34){
      if (*buffer == 9 || *buffer == 10){
        error();
      }
      else if(*buffer == 92){
        ++buffer;
        if(*buffer == 34 || *buffer == 10 || *buffer == 9 || *buffer == 92){
         ++buffer;
        }
        else
         error();
      }
      else
        ++buffer;
    }
    t.c = *buffer++;
    t.type = Token::STRING;
    t.lexeme = string(beginLexeme, buffer);
    return t;
  }

  if (*buffer == 'T')
  {
   ++buffer;
    if(*buffer == 'R'){
      ++buffer;
      if(*buffer == 'U'){
        ++buffer;
          if(*buffer == 'E'){
            t.c = *buffer++;
            t.type = Token::TRUE;
            t.lexeme = string(beginLexeme, buffer);
            return t;
          }
       }
    }
  }

  if (*buffer == 'F')
  {
    ++buffer;
    if(*buffer == 'A'){
      ++buffer;
      if(*buffer == 'L'){
        ++buffer;
          if(*buffer == 'S'){
            ++buffer;
            if(*buffer == 'E'){
              t.c = *buffer++;
              t.type = Token::FALSE;
              t.lexeme = string(beginLexeme, buffer);
              return t;
            }
          }
      }
    }
  }


  if (*buffer == 'N')
  {
    ++buffer;
    if(*buffer == 'U'){
      ++buffer;
      if(*buffer == 'L'){
        ++buffer;
          if(*buffer == 'L'){
            t.c = *buffer++;
            t.type = Token::NULL_;
            t.lexeme = string(beginLexeme, buffer);
            return t;
          }
       }
     }
   }
  char c = *buffer++;

  switch (c)
  {
    case '{':
    case '}':
    case '[':
    case ']':
    case '+':
    case ',':
    case ':':
    case '!':
      ++buffer;
      t.type = c;
      t.lexeme = string(beginLexeme, buffer);
      return t;
  }

printf("%d\n", t.type);
printf("%s\n", t.lexeme);
  error();
  t.type = Token::ERROR;
  return t;
}

/***********************
void Parser::MichaelScott()
{
  Components();
  if (lookahead.type == Token::EOF_)
    advance();
  else
    error();
}

void Parser::Components()
{
    switch (lookahead.type){
        case '!':
            Include();
            break;
        case 'g':
            Declaration();
            break;
        case 'D':
            Function();
            break;
        case 'S':
            Scranton();
            break;
        default:
            error();
    }
}

void Parser::Include()
{
    cout << "Include" << endl;
}
void Parser::Declaration()
{
    cout << "Declaration" << endl;
}
void Parser::Function()
{
    cout << "Function" << endl;
}
void Parser::Scranton()
{
    cout << "Scranton" << endl;
}
void
Parser::Value()
{
  switch (lookahead.type)
  {
    case '{':
      Object();
      break;
    case '[':
      Array();
      break;
    case Token::STRING:
    case Token::NUMBER:
    case Token::TRUE:
    case Token::FALSE:
    case Token::NULL_:
      advance();
      break;
    default:
      error();
  }
}

void
Parser::Object()
{
  if (lookahead.type == '{'){
    objetos++;
    match('{');
    MembersOpt();
    match('}');
  }
  else
    error();
}

void
Parser::MembersOpt()
{
  if (lookahead.type == Token::STRING){
    Members();
  }
  else
    ;
}

void
Parser::Members()
{
  membros++;
  Pair();
  M_();
}

void
Parser::M_()
{
  if (lookahead.type == ','){
    membros++;
    match(',');
    Pair();
    M_();
  }
  else
    ;
}

void
Parser::Pair()
{
  if (lookahead.type == Token::STRING){
    advance();
    match(':');
    Value();
  }
  else
    error();
}

void
Parser::Array()
{
  if (lookahead.type == '['){
    arrays++;
    match('[');
    ElementsOpt();
    match(']');
  }
  else
    error();
}

void
Parser::ElementsOpt()
{
  switch (lookahead.type)
  {
    case '{':
    case '[':
    case Token::STRING:
    case Token::NUMBER:
    case Token::TRUE:
    case Token::FALSE:
    case Token::NULL_:
      Elements();
      break;
    default:
      ;
  }
}

void
Parser::Elements()
{
  elementos++;
  switch (lookahead.type)
  {
    case '{':
    case '[':
    case Token::STRING:
    case Token::NUMBER:
    case Token::TRUE:
    case Token::FALSE:
    case Token::NULL_:
      Value();
      E_();
      break;
    default:
      error();
  }
}

void
Parser::E_()
{
  if (lookahead.type == ','){
    elementos++;
    match(',');
    Value();
    E_();
  }
  else
    ;
}
*/
#endif // __lexic_h