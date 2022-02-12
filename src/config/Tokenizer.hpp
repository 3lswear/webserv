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
		OPEN_BRACKET,
		CLOSE_BRACKET,
		MAP_DECL,
		MAPARRAY_DECL,
		COMMENT,
		NO_TOK
	};

	struct s_token
	{
		std::string value;
		e_token type;
		/* std::string to_string(void); */
	};

	bool isspace(char c);

	bool istomlkey(char c);

	bool istomlmapdecl(char c);

	class Tokenizer
	{
		private:
			std::fstream file;
			size_t prev_pos;
			e_token last_token;

		public:
			Tokenizer(char *filename);
			char getWithoutWhiteSpace();
			struct s_token getToken();
			bool hasMoreTokens();
			bool firstToken();
			void rollBackToken();
			void set_last(e_token type);

			class NoMoreTokens: public std::exception
			{
				public:
					virtual const char *what() const throw()
					{
						return ("No more tokens!");
					}
			};
			class InvalidToken: public std::exception
			{
				protected:
					std::string *msg;

				public:
					InvalidToken(const std::string &token)
					{

						msg = new std::string("Invalid token: '" + token + "'");
					}

					virtual const char *what() const throw()
					{
						return (msg->c_str());
					}

					virtual ~InvalidToken() throw()
					{
						delete msg;
					}
			};

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
