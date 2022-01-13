#ifndef TOMLNODE_HPP
#define TOMLNODE_HPP

#include "webserv.hpp"
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
		
		enum e_type get_type(void)
		{
			return (type);
		}

		TOMLMap *getMap(void)
		{
			return (value.map);
		}

		TOMLMapArray *getMapArray(void)
		{
			return (value.map_array);
		}

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
		void setObject(TOMLMap *obj)
		{
			value.map = obj;
			type = MAP;
		}
		void setMapArray(TOMLMapArray *map_array)
		{
			value.map_array = map_array;
			type = MAPARRAY;
		}
		std::string *toString(void) const
		{
			switch (type)
			{
				case STRING:
				{
					return (value.str);
				}
				case NUM:
				{
					std::stringstream ss;
					ss << value.integer;
					std::string *result = new std::string();
					ss >> *result;
					return (result);
				}
				case ARRAY:
				{
					TOMLArray::iterator it;
					std::string *result = new std::string("[ ");
					for (it = value.array->begin(); it != value.array->end(); ++it)
					{
						*result += *((*it)->toString());
						*result += ", ";
					}
					*result += " ]";
					return (result);
				}
				case BOOL:
				{
					std::string *result;
					if (value.boolean)
						result = new std::string("true");
					else
						result = new std::string("false");
					return (result);
				}
				default:
					return ( new std::string("Not implemented :)"));
			}
		}
};

#endif
