#ifndef TOMLSTUFF_HPP
#define TOMLSTUFF_HPP

class toml_node;

typedef std::map<std::string, toml_node *> TOMLMap; // = JSONObject
typedef std::vector<TOMLMap *> TOMLMapArray;
typedef std::vector<toml_node *> TOMLArray;

#endif

