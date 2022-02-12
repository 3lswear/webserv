#include "Tokenizer.hpp"

/* -Template::-Template(void) */
/* { */
/* 	return; */
/* } */

/* -Template::-Template(const -Template &src) */
/* { */
/* 	*this = src; */
/* 	return; */
/* } */

/* -Template::~-Template(void) */
/* { */
/* 	return; */
/* } */

/* -Template &-Template::operator=(const -Template &rhs) */
/* { */
/* 	//code */
/* 	return (*this); */
/* } */
namespace config
{
	bool isspace(char c)
	{
		if (c == ' ' || c == '\t')
			return (true);
		else
			return (false);
	}

	bool istomlkey(char c)
	{
		if (isalnum(c) || c == '-' || c == '_')
			return (true);
		else
			return (false);
	}
	Tokenizer::Tokenizer(char *filename)
	{
		last_token = NO_TOK;
		file.open(filename, std::ios::in);
		if (!file.good())
		{
			std::cerr << "file didn't open" << std::endl;
			throw std::logic_error("file didnt open");
		}
	}
	bool Tokenizer::firstToken()
	{
		// doesn't account for indent!
		if (file.tellg() == 0 || file.tellg() == 1 || (last_token == NEWLINE || last_token == NO_TOK))
			return (true);
		else
			return (false);
	}

	struct s_token Tokenizer::getToken(void)
	{
		char c;
		struct s_token token;

		if (file.eof())
		{
			std::cout << "Tokens exhausted" << std::endl;
			throw std::logic_error("Tokens exhausted");
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
		else if (c == '[' && firstToken())
		{
			/* std::streampos prev_pos = file.tellg(); */
			file.get(c);
			if (c == '[')
			{
				token.type = MAPARRAY_DECL;
				file.get(c);
				while (c != ']')
				{
					token.value += c;
					file.get(c);
				}
				if (c == ']')
					file.get(c);
				if (c != ']')
					throw std::logic_error("error in MAPARRAY_DECL");
			}
			else
			{
				token.type = MAP_DECL;
				token.value += c;
				file.get(c);
				while (c != ']')
				{
					token.value += c;
					file.get(c);
				}
				if (c != ']')
					throw std::logic_error("malformed MAP_DECL");
			}
		}
		else if (c == '[')
		{
			token.type = OPEN_BRACKET;
		}
		else if (c == ']')
			token.type = CLOSE_BRACKET;
		else if (c == '=')
			token.type = ASSIGN;
		else if (c == '\n')
		{
			token.type = NEWLINE;

			do
				file.get(c);
			while (c == '\n' && !file.eof());
			if (file.eof())
			{
				file.clear();
				DBOUT << "cleared" <<ENDL;
			}
			else if (c != '\n')
				file.seekg(-1, std::ios_base::cur);

		}
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
			while (std::isalpha(c))
			{
				token.value += c;
				file.get(c);
			}
			file.seekg(-1, std::ios_base::cur);

		}
		else if (c == 't')
		{
			token.type = BOOL;
			while (std::isalpha(c))
			{
				token.value += c;
				file.get(c);
			}
			file.seekg(-1, std::ios_base::cur);
		}
		else if (c == 'n')
		{
			token.type = NIL;
			file.seekg(3, std::ios_base::cur);
		}
		else if (c == ',')
			token.type = COMMA;
		else if (c == '#')
		{
			// consume all comments
			do
				file.get(c);
			while (c != '\n' || file.eof());
			DBOUT << "getting comment token" << ENDL;
			if (last_token == NO_TOK || last_token == NEWLINE)
			{
				DBOUT << "getting first token instead of comment" << ENDL;
				struct s_token actual;
				actual.type = NEWLINE;
					while (actual.type == NEWLINE)
						actual = getToken();
					DBOUT
						<< "actual token: '"
						<< actual.value << "', type: "
						<< actual.type
						<< ENDL;
				token = actual;
			}
			else
				token.type = NEWLINE;

		}
		last_token = token.type;

		DBOUT << YELLO << "GOT " << token.value << ", type: " << token.type << ENDL;

		return (token);
	}

	char Tokenizer::getWithoutWhiteSpace(void)
	{
		char c = ' ';
		while (config::isspace(c))
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

	void Tokenizer::set_last(e_token type)
	{
		last_token = type;
	}
}
