#include "TOMLNode.hpp"

toml_node::e_type toml_node::get_type(void)
{
	return (type);
}

std::string *toml_node::getString(void) const
{
	return (value.str);
}

int toml_node::getNum(void) const
{
	return (value.integer);
}

bool toml_node::getBool(void) const
{
	return (value.boolean);
}

TOMLArray *toml_node::getArray(void) const
{
	return (value.array);
}

TOMLMap *toml_node::getMap(void)
{
	return (value.map);
}

TOMLMapArray *toml_node::getMapArray(void)
{
	return (value.map_array);
}

void toml_node::setString(std::string *str)
{
	value.str = str;
	type = STRING;
}

void toml_node::setNumber(int num)
{
	value.integer = num;
	type = NUM;
}

void toml_node::setArr(TOMLArray *toml_array)
{
	value.array = toml_array;
	type = ARRAY;
}
void toml_node::setBool(bool b)
{
	value.boolean = b;
	type = BOOL;
}
void toml_node::setNil(void)
{
	type = NIL;
}

void toml_node::setObject(TOMLMap *obj)
{
	value.map = obj;
	type = MAP;
}

void toml_node::setMapArray(TOMLMapArray *map_array)
{
	value.map_array = map_array;
	type = MAPARRAY;
}


std::string toml_node::toString(void) const
{
	switch (type)
	{
		case STRING:
			{
				return (*value.str);
			}
		case NUM:
			{
				std::stringstream ss;
				ss << value.integer;
				std::string result;
				ss >> result;
				return (result);
			}
		case ARRAY:
			{
				TOMLArray::iterator it;
				std::string result("[ ");
				for (it = value.array->begin(); it != value.array->end(); ++it)
				{
					result += ((*it)->toString());
					result += ", ";
				}
				result += " ]";
				return (result);
			}
		case MAP:
			{
				return (TOMLMap_to_string(value.map));
			}
		case MAPARRAY:
			{
				std::stringstream ss;
				std::string result;
				TOMLMapArray::iterator it;
				TOMLMapArray *map_array = value.map_array;

				ss << "[\n";
				for (it = map_array->begin(); it != map_array->end(); ++it)
				{
					ss << (TOMLMap_to_string(*it));
					ss << ", " << std::endl;
				}
				ss << "]\n";


				/* ss >> *result; */
				result = ss.str();
				return (result);
			}
		case BOOL:
			{
				std::string result;
				if (value.boolean)
					result = std::string("true");
				else
					result = std::string("false");
				return (result);
			}
		default:
			return (std::string("Not implemented :)"));
	}
}

std::string toml_node::TOMLMap_to_string(TOMLMap *map)
{
	std::stringstream ss;
	std::string result;
	TOMLMap::iterator it;

	ss << "{\n";
	for (it = map->begin(); it != map->end(); ++it)
	{
		ss << it->first
			<< ": "
			<< (it->second->toString())
			<< std::endl;
	}

	ss << "}" << std::endl;

	/* ss >> *result; */
	result = ss.str();
	return (result);
}
