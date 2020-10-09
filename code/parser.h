#ifndef __parser_h
#define __parser_h

#include "lexic.h"
#include "michaelscottgrammar.h"
#ifndef __vector
  #define __vector
  #include <vector>
#endif

class Parser{
public:
  MichaelScott* parse(const char*);
  Parser(){};
private:
  char* buffer;
  int lineNumber;
  Token lookahead;

  void error(const char* error_warning);
  void error(const char* error_warning, std::string error_text);
  void error();
  Token nextToken();
  int parserSupport(const char* support_text);

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
  MichaelScott* michaelscott();

}; // Parser


void Parser::error()
{
  printf("**Error [line %d]: unknown\n", lineNumber);
  exit(1);
}

void Parser::error(const char* error_warning)
{
  printf("**Error [line %d]: %s\n", lineNumber, error_warning);
  exit(1);
}

void Parser::error(const char* error_warning, std::string error_text)
{
  printf("**Error [line %d]: %s\n\t%s\n", lineNumber, error_warning, error_text.c_str());
  exit(1);
}

// It parses a support text
int Parser::parserSupport(const char* support_text){
  std::string buff{""};
  std::vector<std::string> text_aux;
  
  // Splits the support text with space delimiter
  for(auto n:std::string(support_text)){
    if(n != ' ') 
      buff+=n; 
    else if(n == ' ' && buff != ""){
      text_aux.push_back(buff); 
      buff = ""; 
    }
  }
  if(buff != "") 
    text_aux.push_back(buff);

  int index = 0;
  std::string identifier_aux;
  char* buffer_aux;
  bool correct;
  int parser_aux = 0;
  buffer_aux = buffer;
  IdentifierStr = "";
  do{
    correct = false;
    identifier_aux = *buffer_aux;
    while (isalnum((*buffer_aux++))){
      identifier_aux += *buffer_aux;
    }
    if(text_aux[index] == identifier_aux.substr(0, identifier_aux.size()-1)){
      IdentifierStr += identifier_aux;
      correct = true;
      parser_aux++;
    }
    index++;
  }while(correct && index < text_aux.size());

  if(correct){
    IdentifierStr = IdentifierStr.substr(0, IdentifierStr.size()-1);
    buffer = buffer_aux;
    parser_aux = 1;
  }
  else if(parser_aux > 0) parser_aux = -1;
  else parser_aux = 0;
  return parser_aux;
}

Token Parser::nextToken()
{
  while (isspace(*buffer)){
    if(*buffer == '\n') lineNumber++;
    *buffer++;
  }

  if (*buffer == '\0') return Token(Token::EOF_, std::string("the end"));
  
  char c = *buffer++;
  char c2;
  if(c == '(' || c == ')' || c == ':' || c == '!' || c == ',')
    return Token(Token::tok_symbol, std::string(1, c));
  else if(c == '+' || c == '/' || c == '*' || c == '%')
    return Token(Token::tok_mathop, std::string(1, c));
  else if(c == '<' || c == '>'){
    c2 = *buffer++;
    if(c2 == '=')
      return Token(Token::tok_comphop, std::string()+c+c2);
    *buffer--;
    return Token(Token::tok_comphop, std::string()+c);
  }
  else if(c == '='){
    c2 = *buffer++;
    if(c2 == '=' || c2 == '!')
      return Token(Token::tok_comphop, std::string()+c+c2);
    *buffer--;
    return Token(Token::tok_attr, std::string(1, c));
  }
  *buffer--;

  int support_aux;
  if (isalpha(*buffer)) {
    // If it is a library include
    if(support_aux = parserSupport("SABRE ACQUIRED")){
      if(support_aux < 0)
        error("Wrong support for library include. It should be 'SABRE ACQUIRED'", IdentifierStr);
      *buffer--;
      return Token(Token::tok_support, IdentifierStr);
    }
    // If it is end of library include
    if(support_aux = parserSupport("and Dunder Mifflin")){
      if(support_aux < 0)
        error("Wrong support for library include. It should be 'and Dunder Mifflin'", IdentifierStr);
      *buffer--;
      return Token(Token::tok_support, IdentifierStr);
    }
    // If it is a function return
    if(support_aux = parserSupport("DUNDIE GOES TO")){
      if(support_aux < 0)
        error("Wrong support for function return. It should be 'DUNDIE GOES TO'", IdentifierStr);
      *buffer--;
      return Token(Token::tok_support, IdentifierStr);
    }
    // If it is a function definition
    if(support_aux = parserSupport("DUNDER MIFFLIN THIS IS")){
      if(support_aux < 0)
        error("Wrong support for function definition. It should be 'DUNDER MIFFLIN THIS IS'", IdentifierStr);
      *buffer--;
      return Token(Token::tok_support, IdentifierStr);
    }
    // If it is a If statement
    if(support_aux = parserSupport("IF TOBY IS NOT HERE and")){
      if(support_aux < 0)
        error("Wrong If statement support. It should be 'IF TOBY IS NOT HERE and'");
      *buffer--;
      return Token(Token::tok_support, IdentifierStr);
    }
    // If it is Else If statement
    if(support_aux = parserSupport("TOBY IS HERE OH WAIT IT WASNT HIM SO IF")){
      if(support_aux > 0){
        *buffer--;
        return Token(Token::tok_support, IdentifierStr);
      }
    }
    // If it is Else statement
    if(support_aux = parserSupport("TOBY IS COMING")){
      if(support_aux > 0){
        *buffer--;
        return Token(Token::tok_support, IdentifierStr);
      }
    }
    // If it is end of If statement
    if(support_aux = parserSupport("TOBY ARRIVED")){
      if(support_aux < 0)
        error("Wrong If statement end support. It should be 'TOBY ARRIVED'. If you meant an Else statement support, it should be 'TOBY IS COMING'. If you ment a Else If statement support, it should be 'TOBY IS HERE OH WAIT IT WASNT HIM SO IF'");
      *buffer--;
      return Token(Token::tok_support, IdentifierStr);
    }
    // If it is For statement
    if(support_aux = parserSupport("FOR EACH ANGELAS CAT")){
      if(support_aux < 0)
        error("Wrong For statement support. It should be 'FOR EACH ANGELAS CAT'");
      *buffer--;
      return Token(Token::tok_support, IdentifierStr);
    }
    // If it is end of For statement
    if(support_aux = parserSupport("DWIGHT HAVE KILLED THEM")){
      if(support_aux < 0)
        error("Wrong For statement end support. It should be 'DWIGHT HAVE KILLED THEM'");
      *buffer--;
      return Token(Token::tok_support, IdentifierStr);
    }
    // If it is While statement
    if(support_aux = parserSupport("WHILE STANLEY IS SLEEPING and")){
      if(support_aux > 0){
        *buffer--;
        return Token(Token::tok_support, IdentifierStr);
      }
    }
    // If it is end of While statement
    if(support_aux = parserSupport("THE WORKING DAY IS OVER")){
      if(support_aux < 0)
        error("Wrong While statement end support. It should be 'THE WORKING DAY IS OVER'");
      *buffer--;
      return Token(Token::tok_support, IdentifierStr);
    }
    // If it is Do While statement
    if(support_aux = parserSupport("RYAN DOES")){
      if(support_aux < 0)
        error("Wrong Do While statement support. It should be 'RYAN DOES'");
      *buffer--;
      return Token(Token::tok_support, IdentifierStr);
    }
    // If it is end of Do While statement
    if(support_aux = parserSupport("WHILE KELLY IS TALKING and")){
      if(support_aux < 0)
        error("Wrong Do While statement end support. It should be 'WHILE KELLY IS TALKING and'. If you meant an While statement support, it should be 'WHILE STANLEY IS SLEEPING and'");
      *buffer--;
      return Token(Token::tok_support, IdentifierStr);
    }
    // If it is a function call
    if(support_aux = parserSupport("THATS WHAT SHE SAID")){
      if(support_aux < 0)
        error("Wrong support for function call. It should be 'THATS WHAT SHE SAID'");
      return Token(Token::tok_support, IdentifierStr);
    }
    // If it is False
    if(support_aux = parserSupport("ASSISTANT TO THE REGIONAL MANAGER")){
      if(support_aux < 0)
        error("Wrong False definition. It should be 'ASSISTANT TO THE REGIONAL MANAGER'");
      *buffer--;
      return Token(Token::tok_true, IdentifierStr);
    }
    // If it is True
    if(support_aux = parserSupport("WORLDS BEST BOSS")){
      if(support_aux < 0)
        error("Wrong True definition. It should be 'WORLDS BEST BOSS'");
      *buffer--;
      return Token(Token::tok_true, IdentifierStr);
    }
    // If it is main
    if(support_aux = parserSupport("Scranton")){
      *buffer--;
      return Token(Token::tok_main, IdentifierStr);
    }
    // If it is end of function
    if(support_aux = parserSupport("FIRE")){
      *buffer--;
      return Token(Token::tok_support, IdentifierStr);
    }
    // If it is AND
    if(support_aux = parserSupport("AND")){
      *buffer--;
      return Token(Token::tok_logicop, IdentifierStr);
    }
    // If it is OR
    if(support_aux = parserSupport("OR")){
      *buffer--;
      return Token(Token::tok_logicop, IdentifierStr);
    }
    // If it is NOT
    if(support_aux = parserSupport("NOT")){
      *buffer--;
      return Token(Token::tok_logicop, IdentifierStr);
    }
    // If it is a global variable
    if(support_aux = parserSupport("CORPORATE")){
      return Token(Token::tok_type, IdentifierStr);
    }
    // If it is a short variable type
    if(support_aux = parserSupport("KELLY")){
      return Token(Token::tok_type, IdentifierStr);
    }
    // If it is a long variable type
    if(support_aux = parserSupport("GABE")){
      return Token(Token::tok_type, IdentifierStr);
    }
    // If it is a void variable type
    if(support_aux = parserSupport("CREED")){
      return Token(Token::tok_type, IdentifierStr);
    }
    // Get the identifier
    IdentifierStr = *buffer;
    while (isalnum((*buffer++))){
      IdentifierStr += *buffer;
    }
    IdentifierStr = IdentifierStr.substr(0, IdentifierStr.size()-1);
    *buffer--;
    // If it is a function name
    if(!IdentifierStr.compare(0, 6, "chilli")){
      return Token(Token::tok_function, IdentifierStr);
    }
    // If it is a variable name
    if(!IdentifierStr.compare(0, 5, "paper")){
      return Token(Token::tok_variable, IdentifierStr);
    }
    error("Wrong support text, type name, variable name or function name.");
  }
/*
  if (isdigit(*buffer) || *buffer == '.') { // Number: [0-9.]+
    std::string NumStr;
    do {
      NumStr += *buffer;
      *buffer++;
    } while (isdigit(*buffer) || *buffer == '.');

    NumVal = strtod(NumStr.c_str(), nullptr);
    thisToken.type = -1000;
    thisToken.lexeme = NumStr;
    return thisToken;
  }
*/
  char* begin_digit = buffer;
  // If it is a digit (it can begins with . - [0-9])
  if (*buffer == '.' || *buffer == '-' || isdigit(*buffer))
  {
    if(*buffer == '-')
      buffer++;

    if(*buffer != '0'){
      ++buffer;
      while(isdigit(*buffer))
        ++buffer;
    }
    else
        ++buffer;


    if(*buffer == '.'){
      buffer++;
      if(isdigit(*buffer))
        buffer++;
      else
	      error("Wrong number.", std::string(begin_digit, buffer));
      while (isdigit(*buffer))
        buffer++;
    }

    if(*buffer == 'e' || *buffer == 'E'){
      buffer++;
      if(*buffer == '+' || *buffer == '-')
      buffer++;
      if(isdigit(*buffer))
        buffer++;
      else
	      error("Wrong number.", std::string(begin_digit, buffer));
      while (isdigit(*buffer))
        buffer++;
    }
    return Token(Token::tok_number, std::string(begin_digit, buffer));
  }

  char* begin_string = buffer;
  // If it is a string
  if(*buffer == '"'){
    buffer++;
    while(*buffer != '"'){
      if (*buffer == 9 || *buffer == 10){
        error();
      }
      else if(*buffer == 92){
        buffer++;
        if(*buffer == 34 || *buffer == 10 || *buffer == 9 || *buffer == 92){
         buffer++;
        }
        else
         error();
      }
      else
        buffer++;
    }
    buffer++;
    return Token(Token::tok_string, std::string(begin_string, buffer));
  }

  // If it is a comment
  if (*buffer == '#') {
    do
      *buffer++;
    while(*buffer != EOF && *buffer != '\n' && *buffer != '\r');
    if(*buffer != EOF) return nextToken();
  }

	error();
  return Token(Token::ERROR, "");
}

MichaelScott* Parser::parse(const char* input)
{
  buffer = const_cast<char*>(input);
  lineNumber = 1;
  do{
    lookahead = nextToken();
    lookahead.print();
  }while(lookahead.type != Token::EOF_);
  MichaelScott* MC = michaelscott();
  if (lookahead.type != Token::EOF_)
    error();
  return MC;
}

MichaelScott* Parser::michaelscott()
{
	MichaelScott* mc = new MichaelScott();
	if (lookahead.type == Token::EOF_)
		advance();
	else
		error();
    return mc;
}


#endif // __parser_h