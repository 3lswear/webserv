#include "webserv.hpp"
#include <map>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <exception>

#include "parse.hpp"
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

TOMLMap *parse(char *filename)
{
	// std::string fiцename = "config/simple.toml";
	config::TOMLParser parser(filename);
	TOMLMap *root = parser.parse();
	/* config::display(root); */
	return (root);
}
