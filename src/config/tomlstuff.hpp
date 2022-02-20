#ifndef TOMLSTUFF_HPP
#define TOMLSTUFF_HPP

class toml_node;

typedef std::map<std::string, toml_node *> TOMLMap; // = JSONObject
typedef std::vector<TOMLMap *> TOMLMapArray;
typedef std::vector<toml_node *> TOMLArray;

namespace config
{
	void display(TOMLMap *config);
	void clean_parsed(TOMLMap *root);
	void clean_generic(toml_node *node);
}

#endif

