/*
 * OSG Workbench
 * Ryven 2006
 *
 * Fast String Tokenizer
 **/

#ifndef __STRING_TOKEN_
#define __STRING_TOKEN_

#include <sstream>
#include <limits>
#include <iostream>

namespace StringToken
{
//just like strcpy but replaces \0 with \n
char * strcpyAndNewLine(char * destination, char * origin);

// Remove spaces at the beginig of the string
char * trimLeft(char * string);

// counts the number of tokens in a string, an empty string has 0 token
int numberOfTokens(char * string);

// moves to the next token
char * nextToken(char * string);

// moves to the "token" token
char * getToken(char * string, int token);

// copy the next token, and moves to the next token
char * copyToken(char *dest, char *source);

// removes '\n' from a input stream 
std::istream &cleanLine(std::istream &input);

/**
 * striped down lexical_cast 
 * 
 * modifed version of boost's  lexical_cast
 * Credits to "the future of c++" Boost lib
 **/
 template<typename Target, typename Source>
   Target lexical_cast(Source const &source)
 {
   Target target;
   
   std::stringstream stream;
   stream.unsetf(std::ios::skipws);
   
   if(std::numeric_limits<Target>::is_specialized)
      stream.precision(std::numeric_limits<Target>::digits10 + 1);
   else if(std::numeric_limits<Source>::is_specialized)
     stream.precision(std::numeric_limits<Source>::digits10 + 1);
   
    stream << source;
    stream >> target;
    
    return target;
 }
};

#endif// __STRING_TOKEN__
