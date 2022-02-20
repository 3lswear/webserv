#ifndef TOMLNODE_HPP
#define TOMLNODE_HPP

#include "webserv.hpp"
#include "tomlstuff.hpp"

#include <map>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <exception>
#include <iostream>
#include <sstream>

class toml_node
{

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
	}	value;

	public:

		enum e_type
		{
			STRING, NUM, BOOL, ARRAY, MAP, MAPARRAY, NIL
		}	type;
		
		enum e_type get_type(void);

		std::string *getString(void) const;
		int getNum(void) const;
		bool getBool(void) const;
		TOMLArray *getArray(void) const;
		TOMLMap *getMap(void);
		TOMLMapArray *getMapArray(void);

		void setString(std::string *str);
		void setNumber(int num);

		void setArr(TOMLArray *toml_array);
		void setBool(bool b);
		void setNil(void);
		void setObject(TOMLMap *obj);
		void setMapArray(TOMLMapArray *map_array);

		std::string toString(void) const;
		static std::string TOMLMap_to_string(TOMLMap *map);
};


#endif
