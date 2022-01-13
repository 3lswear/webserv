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
	void display(toml_node *config)
	{
		std::cout << ">>> printing config: <<<" << std::endl;
		if (config->get_type() != toml_node::MAPARRAY)
			throw std::logic_error("Attempting to display not map_array");

		TOMLMapArray *root_map_array = config->getMapArray();
		TOMLMapArray::iterator root_it;
		TOMLMap::iterator it;

		for (root_it = root_map_array->begin(); root_it != root_map_array->end(); ++root_it)
		{
			for (it = (*root_it)->begin(); it != (*root_it)->end(); ++it)
			{
				std::cout << it->first
					<< ": "
					<< *(it->second->toString())
					<< std::endl;
			}
			std::cout << "-------" << std::endl;
		}
	}
}

void parse(void)
{
	std::string filename = "config/simple.toml";
	config::TOMLParser parser(filename);
	toml_node *root = parser.parse();
	config::display(root);
}
