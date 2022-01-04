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
		std::string to_string(void);
	};

	class Tokenizer
	{
		private:
			std::fstream file;
			size_t prev_pos;
		public:
			Tokenizer(std::string filename);
			char getWithoutWhiteSpace();
			struct s_token getToken();
			void *hasMoreTokens();
			void rollBackToken();
	};

	char Tokenizer::getWithoutWhiteSpace(void)
	{
		char c = ' ';
		while ((c == ' ' || c == '\n'))
		{
			file.get(c);
			if ((c == ' ' || c == '\n') && !file.good())
			{
				throw std::logic_error("No more tokens!");
			}
			else if (!file.good())
				return (c);
		}
		return (c);
	}

	void Tokenizer::rollBackToken(void)
	{
		if (file.eof())
			file.clear();
		file.seekg(prev_pos);
	}

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

		if (c == '"')
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
		}
		else if (c == 't')
		{
			token.type = BOOL;
			token.value = "true";
			file.seekg(3, std::ios_base::cur);
		}
		else if (c == 'n')
		{
			token.type = NIL;
			file.seekg(3, std::ios_base::cur);
		}
		else if (c == ',')
			token.type = COMMA;
		return (token);
	}
}
