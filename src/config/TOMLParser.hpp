#ifndef TOMLPARSER_HPP
#define TOMLPARSER_HPP

#include "webserv.hpp"
#include "tomlstuff.hpp"
#include "Tokenizer.hpp"
#include "TOMLNode.hpp"
#include <string>

namespace config
{
	class TOMLParser
	{
		private:
			std::fstream file;
			TOMLMap *root; //root of TOML tree
			/* toml_node *current; //node currently being parsed */
			Tokenizer tokenizer;

			static std::vector<std::string> split_name(std::string name);
			static void put_to_subtable(TOMLMap *root,
					std::vector<std::string> full_name,
					toml_node *map_node, toml_node::e_type type);

		public:
			TOMLParser(const std::string filename);
			TOMLMap *parse(void);

			toml_node *parseMap(void);
			void processMap(void);

			void processMapArray(void);

			toml_node *parseString(void);

			toml_node *parseNumber(void);

			toml_node *parseArray(void);

			toml_node *parseBool(void);

			toml_node *parseNil(void);

	};

	/* parse tha root ! */
	/* TOMLMap *TOMLParser::parse(void); */
}
#endif
