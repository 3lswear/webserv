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
		DBOUT << ">>> printing config: <<<" << std::endl;

		TOMLMap::iterator it;

		for (it = root_map->begin(); it != root_map->end(); ++it)
		{
			DBOUT << it->first
				<< ": "
				<< (it->second->toString());
			DBOUT << ", " << std::endl;
			/* << std::endl << "-------" << std::endl; */
		}
	}

	void clean_parsed(TOMLMap *root)
	{
		TOMLMap::iterator it;

		// DBOUT << ">>> cleaning up: <<<" << std::endl;
		// DBOUT << root << ENDL;
		if (!root)
			return;
		for (it = root->begin(); it != root->end(); ++it)
		{
			// DBOUT << RED << it->first
			// 	<< ": " << GREEN
			// 	<< *(it->second->toString());

			clean_generic(it->second);
			/* delete it->second; */
			// DBOUT << ", " << std::endl;
		}
		// DBOUT << YELLO << "end of clean" << ENDL;
		root->clear();
		delete root;
		root = NULL;
	}

	void clean_generic(toml_node *node)
	{
		switch (node->type)
		{
			case toml_node::STRING:
			{
				// DBOUT << "cleaning string" << ENDL;
				delete node->getString();
			}
			break;
			case toml_node::MAPARRAY:
			{
				// DBOUT << "cleaning MAPARRAY" << ENDL;
				TOMLMapArray *map_array = node->getMapArray();
				for (TOMLMapArray::iterator it = map_array->begin();
						it != map_array->end(); ++it)
				{
					// DBOUT << "cleaning a MAP of MAPARRAY" << ENDL;
					TOMLMap *map = *it;
					TOMLMap::iterator map_it = map->begin();
					for (map_it = map->begin();
							map_it != map->end(); ++map_it)
					{
						// DBOUT << "cleaning a MAP item " << map_it->first << ENDL;
						config::clean_generic(map_it->second);
					}
					map->clear();
					delete map;
				}
				map_array->clear();
				delete map_array;
				// DBOUT << "end cleaning MAPARRAY" << ENDL;
			}
			break;
			case toml_node::MAP:
			{
				// DBOUT << "cleaning MAP" << ENDL;
				TOMLMap *map = node->getMap();
				for (TOMLMap::iterator it = map->begin(); it != map->end(); ++it)
					config::clean_generic(it->second);
				map->clear();
				delete map;
			}
			break;

			case toml_node::ARRAY:
			{
				// DBOUT << "cleaning ARRAY" << ENDL;
				TOMLArray *arr = node->getArray();
				for (TOMLArray::iterator it = arr->begin();
						it != arr->end(); ++it)
					config::clean_generic(*it);
				arr->clear();
				delete arr;
				// DBOUT << "end cleaning MAP" << ENDL;
			}
			break;

			default:
			{
				// DBOUT << "Cleaning type " << node->type << " not implemented :)" << ENDL;
			}
		}
		delete node;

	}
}

// TOMLMap *parse(char *filename)
// {
// 	// std::string fiцename = "config/simple.toml";
// 	// config::TOMLParser parser(filename);
// 	// parser.parse();
// 	// /* config::display(root); */
// 	// return (parser.root);
// }
