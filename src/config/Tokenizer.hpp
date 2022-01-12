#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include "webserv.hpp"
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
		MAPARRAY_OPEN,
		MAPARRAY_CLOSE
	};

	struct s_token
	{
		std::string value;
		e_token type;
		/* std::string to_string(void); */
	};

	bool istomlkey(char c)
	{
		if (isalnum(c) || c == '-' || c == '_')
			return (true);
		else
			return (false);
	}

	class Tokenizer
	{
		private:
			std::fstream file;
			size_t prev_pos;
			e_token last_token;
		public:
			Tokenizer(std::string filename)
			{
				file.open(filename.c_str(), std::ios::in);
				if (!file.good())
				{
					std::cerr << "file didn't open" << std::endl;
				}
			}
			char getWithoutWhiteSpace();
			struct s_token getToken();
			bool hasMoreTokens();
			bool firstToken()
			{
				// doesn't account for indent!
				if (file.tellg() == 0 || file.tellg() == 1 || (last_token == NEWLINE))
					return (true);
				else
					return (false);
			}
			void rollBackToken();
	};

	char Tokenizer::getWithoutWhiteSpace(void)
	{
		char c = ' ';
		while (c == ' ')
		{
			file.get(c);
			if ((c == ' ') && !file.good())
			{
				throw std::logic_error("No more tokens!");
			}
			else if (!file.good())
				return (c);
		}
		return (c);
	}

	bool Tokenizer::hasMoreTokens(void)
	{
		return (!file.eof());
	}

	void Tokenizer::rollBackToken(void)
	{
		if (file.eof())
			file.clear();
		file.seekg(prev_pos);
	}

	/* struct s_token Tokenizer::getKey(void) */
	/* { */
	/* 	char c; */
	/* 	struct s_token token; */
	/* 	if (file.eof()) */
	/* 	{ */
	/* 		std::cout << "Tokens exhausted" << std::endl; */
	/* 	} */
	/* } */

	struct s_token Tokenizer::getToken(void)
	{
		char c;
		struct s_token token;

		if (file.eof())
		{
			std::cout << "Tokens exhausted" << std::endl;
		}
		prev_pos = file.tellg();
		c = getWithoutWhiteSpace();

		if (firstToken() && config::istomlkey(c))
		{
			token.type = KEY;
			while (config::istomlkey(c))
			{
				token.value += c;
				file.get(c);
			}
		}
		else if (c == '"')
		{
			token.type = STRING;
			token.value = "";
			/* TODO: maybe do-while? */
			file.get(c);
			while (c != '"')
			{
				token.value += c;
				file.get(c);
			}
		}
		else if (c == '[')
			token.type = ARR_OPEN;
		else if (c == ']')
			token.type = ARR_CLOSE;
		else if (c == '=')
			token.type = ASSIGN;
		else if (c == '\n')
			token.type = NEWLINE;
		else if (c == '-' || isdigit(c))
		{
			std::streampos prevCharPos;
			token.type = NUMBER;
			token.value = "";
			token.value += c;
			/* prevCharPos = file.tellg(); */
			while (c == '-' || c == '.' || isdigit(c))
			{
				prevCharPos = file.tellg();
				file.get(c);
				if (file.eof())
					break;
				else
				{
					if (c == '-' || c == '.' || isdigit(c))
						token.value += c;
					else
						file.seekg(prevCharPos);

				}
			}
		}
		else if (c == 'f')
		{
			token.type = BOOL;
			token.value = "false";
			file.seekg(4, std::ios_base::cur);

			/* token.value = ""; */
			/* while (std::isalpha(c)) */
			/* { */
			/* 	token.value += c; */
			/* 	file.get(c); */
			/* } */
			std::cerr << "value is: " << token.value << std::endl;
			std::cerr << "c is: " << c << std::endl;
		}
		else if (c == 't')
		{
			token.type = BOOL;
			while (std::isalpha(c))
			{
				token.value += c;
				file.get(c);
			}
			/* file.seekg(3, std::ios_base::cur); */
		}
		else if (c == 'n')
		{
			token.type = NIL;
			file.seekg(3, std::ios_base::cur);
		}
		else if (c == ',')
			token.type = COMMA;
		last_token = token.type;
		return (token);
	}
}

#endif
