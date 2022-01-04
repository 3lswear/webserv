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
	void display(TOMLMap *config)
	{
		std::cout << "printing config:" << std::endl;
		std::cout << (*config)["name"] << std::endl;
		std::cout << (*config)["port"] << std::endl;
	}
}

void parse(void)
{
	std::string filename = "config/simple.toml";
	config::TOMLParser parser(filename);
	toml_node *root = parser.parse();
	config::display(root->getMap());
}
