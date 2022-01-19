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

		static std::string *TOMLMap_to_string(TOMLMap *map)
		{
			std::stringstream ss;
			std::string *result = new std::string();
			TOMLMap::iterator it;

			ss << "{\n";
			for (it = map->begin(); it != map->end(); ++it)
			{
				ss << it->first
					<< ": "
					<< *(it->second->toString())
					<< std::endl;
			}

			ss << "}" << std::endl;

			/* ss >> *result; */
			*result = ss.str();
			return (result);
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
				case MAP:
				{
					return (TOMLMap_to_string(value.map));
				}
				case MAPARRAY:
				{
					std::stringstream ss;
					std::string *result = new std::string();
					TOMLMapArray::iterator it;
					TOMLMapArray *map_array = value.map_array;

					ss << "[\n";
					for (it = map_array->begin(); it != map_array->end(); ++it)
					{
						ss << (*TOMLMap_to_string(*it));
						ss << ", " << std::endl;
					}
					ss << "]\n";


					/* ss >> *result; */
					*result = ss.str();
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
