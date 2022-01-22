#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include "webserv.hpp"
#include "tomlstuff.hpp"

#include <map>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <exception>


namespace config
{
	enum e_token
	{
		KEY,
		NEWLINE,
		ASSIGN,
		STRING,
		NUMBER,
		COMMA,
		BOOL,
		NIL,
		ARR_OPEN,
		ARR_CLOSE,
		MAP_OPEN,
		MAP_CLOSE,
		MAPARRAY_DECL
	};

	struct s_token
	{
		std::string value;
		e_token type;
		/* std::string to_string(void); */
	};

	bool isspace(char c);

	bool istomlkey(char c);

	class Tokenizer
	{
		private:
			std::fstream file;
			size_t prev_pos;
			e_token last_token;

		public:
			Tokenizer(std::string filename);
			char getWithoutWhiteSpace();
			struct s_token getToken();
			bool hasMoreTokens();
			bool firstToken();
			void rollBackToken();

	};

	/* struct s_token Tokenizer::getKey(void) */
	/* { */
	/* 	char c; */
	/* 	struct s_token token; */
	/* 	if (file.eof()) */
	/* 	{ */
	/* 		std::cout << "Tokens exhausted" << std::endl; */
	/* 	} */
	/* } */

}

#endif
