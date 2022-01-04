#include "webserv.hpp"
#include <map>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <exception>


class toml_node;

/* typedef std::vector<TOMLMap *>	TOMLArray; */
/* typedef std::vector<TOMLArray *> TOMLArrayOfMap; */
typedef std::map<std::string, toml_node *> TOMLMap; // = JSONObject
typedef std::vector<std::string, TOMLMap *> TOMLMapArray;
typedef std::vector<toml_node *> TOMLArray;


class toml_node
{
	enum e_type
	{
		STRING, NUM, BOOL, ARRAY, MAP, MAPARRAY, NIL
	} type;

	union u_value
	{
		std::string *str;
		int integer;
		bool boolean;
		/* std::vector<toml_node *> *array; */ 
		TOMLArray *array;
		/* std::map<std::string, toml_node *> *map; */
		TOMLMap *map;
		/* std::vector<std::map<std::string, toml_node> > *map_array; */
		TOMLMapArray *map_array;
	} value;

	public:
		void setString(std::string *str)
		{
			value.str = str;
			type = STRING;
		}

		void setNumber(int num)
		{
			value.integer = num;
			type = NUM;
		}

		void setArr(TOMLArray *toml_array)
		{
			value.array = toml_array;
			type = ARRAY;
		}
		void setBool(bool b)
		{
			value.boolean = b;
			type = BOOL;
		}
		void setNil(void)
		{
			type = NIL;
		}
};
