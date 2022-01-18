#include "webserv.hpp"
#include <map>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <exception>

#include "TOMLNode.hpp"
#include "TOMLParser.hpp"
#include "Tokenizer.hpp"

/* struct location */
/* { */
/* 	std::string location; */
/* 	std::string root; */
	
/* } */

/* class config */
/* { */
/* 	std::string name; */
/* } */
namespace config
{

	void display(TOMLMap *root_map)
	{
		std::cout << ">>> printing config: <<<" << std::endl;

		TOMLMap::iterator it;

		for (it = root_map->begin(); it != root_map->end(); ++it)
		{
			std::cout << it->first
				<< ": "
				<< *(it->second->toString());
			std::cout << ", " << std::endl;
				/* << std::endl << "-------" << std::endl; */
		}
	}
}

void parse(void)
{
	std::string filename = "config/simple.toml";
	config::TOMLParser parser(filename);
	TOMLMap *root = parser.parse();
	config::display(root);
}
