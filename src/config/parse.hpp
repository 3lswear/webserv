#ifndef PARSE_HPP
#define PARSE_HPP

#include "tomlstuff.hpp"

#include "Tokenizer.hpp"
#include "TOMLNode.hpp"
#include "TOMLParser.hpp"

TOMLMap *parse(void);
void display(TOMLMap *config);

#endif
