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

class Token
{
public:
  int type;
  std::string lexeme;

  Token(int type, std::string lexeme):type(type), lexeme(std::move(lexeme)){};
  Token():type(0), lexeme(""){};
  

  void print(){
    std::string type_char;
    if(type == Token::ERROR) type_char = "ERROR";
    else if(type == Token::EOF_) type_char = "EOF";
    else if(type == Token::tok_support) type_char = "support";
    else if(type == Token::tok_statement_support) type_char = "statement_support";
    else if(type == Token::tok_include_support) type_char = "include_support";
    else if(type == Token::tok_function_support) type_char = "function_support";
    else if(type == Token::tok_print_function_support) type_char = "print_support";
    else if(type == Token::tok_scan_function_support) type_char = "scan_support";
    else if(type == Token::tok_global_support) type_char = "global_support";
    else if(type == Token::tok_statement) type_char = "statement";
    else if(type == Token::tok_symbol) type_char = "symbol";
    else if(type == Token::tok_main) type_char = "main";
    else if(type == Token::tok_function) type_char = "function";
    else if(type == Token::tok_print) type_char = "print";
    else if(type == Token::tok_scan) type_char = "scan";
    else if(type == Token::tok_variable) type_char = "variable";
    else if(type == Token::tok_type) type_char = "type";
    else if(type == Token::tok_array_declaration) type_char = "array";
    else if(type == Token::tok_array_declaration_support) type_char = "array_support";
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


  enum TokenType
  {
    ERROR,
    EOF_,
    tok_support,
    tok_statement_support,
    tok_include_support,
    tok_function_support,
    tok_print_function_support,
    tok_scan_function_support,
    tok_global_support,
    tok_statement,
    tok_symbol,
    tok_main,
    tok_function,
    tok_print,
    tok_scan,
    tok_variable,
    tok_type,
    tok_array_declaration,
    tok_array_declaration_support,
    tok_number,
    tok_mathop,
    tok_compop,
    tok_logicop,
    tok_notop,
    tok_attr,
    tok_string,
    tok_true,
    tok_false
  };
};
#endif // __lexic_h