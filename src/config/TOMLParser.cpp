#include "TOMLParser.hpp"

namespace config
{

	TOMLParser::TOMLParser(char *filename) : tokenizer(filename)
	{
		root = new TOMLMap;
	}

	void TOMLParser::processMap(void)
	{
		toml_node *map_node;
		s_token current;

		try { current = tokenizer.getToken(); }
		catch (Tokenizer::NoMoreTokens &e)
		{
			DBOUT << "got no more tokens" << ENDL;
			return;
		}
		if (current.type == MAP_DECL)
		{
			if (tokenizer.getToken().type != NEWLINE)
				throw ExpectedToken("newline", "after " + current.value);
			map_node = parseMap();
		}
		else
			throw UnexpectedToken("", "near " + current.value);

		std::vector<std::string> full_name;

		full_name = split_name(current.value);
		
		try
		{
			put_to_subtable(root, full_name, map_node, toml_node::MAP);;
		}
		catch (std::domain_error &e)
		{
			config::display(root);
			DBOUT << "CAUGHT in process Map" <<ENDL;
			config::clean_generic(map_node);;
			throw;
		}
	}

	toml_node *TOMLParser::parseMap(void)
	{
		toml_node *node = new toml_node;
		TOMLMap *mapObject = new TOMLMap;
		bool completed = false;
		try
		{
			while (!completed)
			{
				if (tokenizer.hasMoreTokens())
				{
					s_token nextToken;
					try { nextToken = tokenizer.getToken(); }
					catch (Tokenizer::NoMoreTokens &e)
					{
						DBOUT << "got no more tokens" << ENDL;
						completed = 1;
						break;
					}

					if (nextToken.type == MAPARRAY_DECL)
					{
						tokenizer.rollBackToken();
						tokenizer.set_last(NEWLINE);
						break;
					}
					else if (nextToken.type == MAP_DECL)
					{
						tokenizer.rollBackToken();
						tokenizer.set_last(NEWLINE);
						break;
					}

					std::string key = nextToken.value;
					if (tokenizer.getToken().type != ASSIGN)
						throw ExpectedToken("assign", "after " + key);

					nextToken = tokenizer.getToken();
					switch (nextToken.type)
					{
						case STRING:
						{
							tokenizer.rollBackToken();
							(*mapObject)[key] = parseString();
							break;
						}
						case OPEN_BRACKET:
						{
							(*mapObject)[key] = parseArray();
							break;
						}
						case NUMBER:
						{
							tokenizer.rollBackToken();
							(*mapObject)[key] = parseNumber();
							break;
						}
						case BOOL:
						{
							tokenizer.rollBackToken();
							(*mapObject)[key] = parseBool();
							break;
						}
						case MAPARRAY_DECL:
						{
							completed = true;
							break;
						}
						default:
							throw UnexpectedToken(nextToken.value, key);
					}
					if (tokenizer.hasMoreTokens())
						nextToken = tokenizer.getToken();
					else
						break;
					if (nextToken.type != NEWLINE)
					{
						throw ExpectedToken("newline", "at parsing Hash Table");
					}
				}
			}
		}
		catch (std::domain_error &e)
		{
			DBOUT << "CAUGHT in parse MAP!!!" <<ENDL;

			node->setObject(mapObject);
			config::clean_generic(node);
			throw;
		}
		node->setObject(mapObject);
		return (node);
	}

	void TOMLParser::processMapArray(void)
	{

		toml_node *map_node;
		s_token current;

		try { current = tokenizer.getToken(); }
		catch (Tokenizer::NoMoreTokens &e)
		{
			DBOUT << "got no more tokens" << ENDL;
			return;
		}
		if (current.type == MAPARRAY_DECL)
		{
			if (tokenizer.getToken().type != NEWLINE)
				throw ExpectedToken("newline", "after " + current.value);
			map_node = parseMap();
		}
		else
			throw UnexpectedToken(current.value, "when expected array of map declaration");

		std::vector<std::string> full_name;

		full_name = split_name(current.value);

		try
		{
			put_to_subtable(root, full_name, map_node, toml_node::MAPARRAY);
		}
		catch (std::domain_error &e)
		{
			config::clean_generic(map_node);
			throw;
		}

	}

	toml_node *TOMLParser::parseString(void)
	{
		std::string *string;

		s_token token = tokenizer.getToken();
		string = new std::string(token.value);

		toml_node *node = new toml_node;
		node->setString(string);

		return (node);
	}

	toml_node *TOMLParser::parseNumber(void)
	{
		int value;

		s_token token = tokenizer.getToken();
		value = std::atoi(token.value.c_str());

		toml_node *node = new toml_node;
		node->setNumber(value);

		return (node);
	}

	toml_node *TOMLParser::parseArray(void)
	{
		toml_node *result = new toml_node;
		TOMLArray *array = new TOMLArray;
		toml_node *node;
		bool completed = false;
		s_token current;

		try
		{
			while (!completed)
			{
				current = tokenizer.getToken();
				switch (current.type)
				{
					case STRING:
					{
						tokenizer.rollBackToken();
						node = parseString();
						break;
					}
					case NUMBER:
					{
						tokenizer.rollBackToken();
						node = parseNumber();
						break;
					}
					case BOOL:
					{
						tokenizer.rollBackToken();
						node = parseBool();
						break;
					}
					case NIL:
					{
						node = parseNil();
						break;
					}
					default:
					{
						throw UnexpectedToken("entry " + current.value,
								"in array");
					}
				}
				array->push_back(node);
				current = tokenizer.getToken();
				if (current.type == COMMA)
					continue;
				else if (current.type == CLOSE_BRACKET)
					completed = true;
				else
					throw UnexpectedToken(current.value, ", when expected COMMA, or CLOSE_BRACKET");
			}
		}
		catch (std::domain_error &e)
		{
			DBOUT << "CAUGHT in parse Array" << ENDL;
			result->setArr(array);
			config::clean_generic(result);
			throw;
		}
		result->setArr(array);
		return (result);
	}

	toml_node *TOMLParser::parseBool(void)
	{
		bool value;

		s_token token = tokenizer.getToken();
		if (token.value == "true")
			value = true;
		else
			value = false;

		toml_node *node = new toml_node;
		node->setBool(value);

		return (node);
	}

	toml_node *TOMLParser::parseNil(void)
	{
		toml_node *node = new toml_node;
		node->setNil();
		return (node);
	}

	/* parse tha root ! */
	void TOMLParser::parse(void)
	{
		bool completed = false;
		while (!completed)
		{
			if (tokenizer.hasMoreTokens())
			{
				s_token current;
				try { current = tokenizer.getToken(); }
				catch (Tokenizer::NoMoreTokens &e)
				{
					DBOUT << "got no more tokens" << ENDL;
					break;
				}
				if (current.type == MAPARRAY_DECL)
				{
					tokenizer.set_last(NEWLINE);
					tokenizer.rollBackToken();
					processMapArray();
				}
				else if (current.type == MAP_DECL)
				{
					tokenizer.set_last(NEWLINE);
					tokenizer.rollBackToken();
					processMap();
					// continue;
				}
			}
			else
				completed = true;
		}
	}

	std::vector<std::string> TOMLParser::split_name(std::string name)
	{
		std::vector<std::string> full_name;
		size_t dot;

		while (1)
		{
			dot = name.find('.');
			if (dot == std::string::npos)
				break;
			full_name.push_back(name.substr(0, dot));
			name.erase(0, dot + 1);
		}
		full_name.push_back(name);

		return (full_name);
	}

	void TOMLParser::put_to_subtable(TOMLMap *root,
			std::vector<std::string> full_name,
			toml_node *map_node, toml_node::e_type type)
	{
		std::vector<std::string>::iterator subname = full_name.begin();
		toml_node *maparr_node;
		TOMLMap::iterator it;

		TOMLMap *local_root = root;

		while (1)
		{
			if (subname + 1 == full_name.end())
			{
				it = local_root->find(*subname);
				if (type == toml_node::MAPARRAY)
				{
					if (it == local_root->end())
					{
						maparr_node = new toml_node;
						TOMLMapArray *map_array = new TOMLMapArray;
						map_array->push_back(map_node->getMap());
						maparr_node->setMapArray(map_array);
						(*local_root)[*subname] = maparr_node;
					}
					else
						(it->second)->getMapArray()->push_back(map_node->getMap());
					delete map_node;
				}
				else if (type == toml_node::MAP)
				{
					if (it == local_root->end())
					{
						(*local_root)[*subname] = map_node;
					}
					else
						throw std::domain_error("Config file: Map " + *subname + " already exists in subtable!");
				}
				break;
			}
			else
			{
				it = local_root->find(*subname);

				/* subname not found in local_root */
				if (it == local_root->end())
				{
					toml_node *map_node2;
					map_node2 = new toml_node;
					TOMLMap *map = new TOMLMap;
					map_node2->setObject(map);
					(*local_root)[*subname] = map_node2;
					local_root = map;
				}
				/* subname found in local_root */
				else
					local_root = *((it->second)->getMapArray()->end() - 1);

			}
			++subname;
		}
	}
}
