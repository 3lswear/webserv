#ifndef TOMLPARSER_HPP
#define TOMLPARSER_HPP

#include "webserv.hpp"
#include "tomlstuff.hpp"
#include "Tokenizer.hpp"
#include "TOMLNode.hpp"
#include <string>
#include <memory>
#include <stdexcept>

namespace config
{
	class TOMLParser
	{
		private:
			std::fstream file;
			/* toml_node *current; //node currently being parsed */
			Tokenizer tokenizer;

			static std::vector<std::string> split_name(std::string name);
			static void put_to_subtable(TOMLMap *root,
					std::vector<std::string> full_name,
					toml_node *map_node, toml_node::e_type type);

		public:
			TOMLParser(char *filename);
			void	parse(void);

			TOMLMap *root; //root of TOML tree

			toml_node *parseMap(void);
			// TOMLMap *parseMap(void);
			void processMap(void);

			void processMapArray(void);

			toml_node *parseString(void);

			toml_node *parseNumber(void);

			toml_node *parseArray(void);

			toml_node *parseBool(void);

			toml_node *parseNil(void);

			// Expected + missing + context
			class ExpectedToken: public std::domain_error
			{
				protected:
					std::string msg;

				public:
					ExpectedToken(const std::string &missing, const std::string context): std::domain_error("ExpectedToken")
					{
						msg = std::string("Config file: ");
						msg += ("Expected " + missing + " " + context);
					}

					virtual const char *what() const throw()
					{
						return (msg.c_str());
					}

					virtual ~ExpectedToken() throw()
					{}
			};

			// Unexpected + unexpected + context
			class UnexpectedToken: public std::domain_error
			{
				protected:
					std::string msg;

				public:
					UnexpectedToken(const std::string &unexpected, const std::string context): std::domain_error("ExpectedToken")
					{
						msg = std::string("Config file: ");
						msg += ("Unexpected " + unexpected + " " + context);
					}

					virtual const char *what() const throw()
					{
						return (msg.c_str());
					}

					virtual ~UnexpectedToken() throw()
					{}

			};
			class AlreadyExists: public std::domain_error
			{
				protected:
					std::string msg;

				public:
					AlreadyExists(const std::string &token, const std::string context): std::domain_error("ExpectedToken")
					{
						msg = std::string("Config file: ");
						msg += (token + "already exists " + context);
					}

					virtual const char *what() const throw()
					{
						return (msg.c_str());
					}

					virtual ~AlreadyExists() throw()
					{}

			};
	};
	/* parse tha root ! */
	/* TOMLMap *TOMLParser::parse(void); */
}
#endif
