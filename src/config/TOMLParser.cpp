#include "TOMLParser.hpp"

/* -Template::-Template(void) */
/* { */
/* 	return; */
/* } */

/* -Template::-Template(const -Template &src) */
/* { */
/* 	*this = src; */
/* 	return; */
/* } */

/* -Template::~-Template(void) */
/* { */
/* 	return; */
/* } */

/* -Template &-Template::operator=(const -Template &rhs) */
/* { */
/* 	//code */
/* 	return (*this); */
/* } */

namespace config
{

	TOMLParser::TOMLParser(char *filename) : tokenizer(filename)
	{
		root = new TOMLMap;
	}

	void TOMLParser::processMap(void)
	{
		/* std::cerr << "Processing map" << std::endl; */
		// toml_node *map_node;
		std::auto_ptr<toml_node> map_node(new toml_node);
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
				throw ExpectedToken("newline", current.value);
				// throw std::logic_error("no newline after MAP_DECL");
			map_node.reset(parseMap());
		}
		else
			// throw std::logic_error("unexpected token in processMap");
			throw UnexpectedToken("", current.value);

		/* std::cout << current.value << std::endl; */

		std::vector<std::string> full_name;

		full_name = split_name(current.value);
		
		put_to_subtable(root, full_name, map_node.release(), toml_node::MAP);;

	}

	toml_node *TOMLParser::parseMap(void)
	{
		/* std::cerr << "Parsing map" << std::endl; */
		// std::auto_ptr<TOMLMap> mapObject(new TOMLMap);
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
					/* std::cerr << key << std::endl; */
					if (tokenizer.getToken().type != ASSIGN)
						throw ExpectedToken("assign", "after " + key);
					// throw std::logic_error("EXPECTED assign! 1");

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
							/* std::cerr << "reached MAPARRAY_DECL in parseMap" << std::endl; */
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
						// throw std::logic_error("EXPECTED newline");
						throw ExpectedToken("newline", "parsing Hash Table");
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

		/* std::cerr << "Parsing MapArray" << std::endl; */
		// toml_node *map_node;
		std::auto_ptr<toml_node> map_node(new toml_node);
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
				throw std::logic_error("no newline after map_array declaration");
			try
			{
				map_node.reset(parseMap());
			}
			catch (std::exception &e)
			{
				// DBOUT << "map_node is " << map_node->getMap() << ENDL;

				// for (TOMLMap::iterator it = map->begin();)
				throw;
			}
		}
		else
			throw std::logic_error("unexpected token in process Map Array");

		/* std::cout << current.value << std::endl; */

		std::vector<std::string> full_name;

		full_name = split_name(current.value);

		put_to_subtable(root, full_name, map_node.release(), toml_node::MAPARRAY);

	}

	toml_node *TOMLParser::parseString(void)
	{
		std::string *string;

		/* std::cerr << "Parsing string" << std::endl; */
		s_token token = tokenizer.getToken();
		string = new std::string(token.value);

		toml_node *node = new toml_node;
		node->setString(string);

		return (node);
	}

	toml_node *TOMLParser::parseNumber(void)
	{
		int value;

		/* std::cerr << "Parsing number" << std::endl; */
		s_token token = tokenizer.getToken();
		value = std::atoi(token.value.c_str());

		toml_node *node = new toml_node;
		node->setNumber(value);

		return (node);
	}

	toml_node *TOMLParser::parseArray(void)
	{
		/* std::cerr << "Parsing array" << std::endl; */
		toml_node *result = new toml_node;
		TOMLArray *array = new TOMLArray;
		toml_node *node;
		bool completed = false;
		s_token current;

		try
		{

		while (!completed)
		{
			// if (!tokenizer.hasMoreTokens())
			// 	throw std::logic_error("No more tokens");
			// else
			// {
				current = tokenizer.getToken();
				switch (current.type)
				{
					// case OPEN_BRACKET:
					// 	{
					// 		node = parseArray();
					// 		break;
					// 	}
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
							// throw std::logic_error("unkown token in parseArray");
							throw UnexpectedToken("entry " + current.value,
									"in Array");
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
					// throw std::invalid_argument("Unexpected token in array!");
			// }
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

		/* std::cerr << "Parsing bool" << std::endl; */
		s_token token = tokenizer.getToken();
		if (token.value == "true")
			value = true;
		else if (token.value == "false")
			value = false;
		else
			throw std::invalid_argument("Unexpected bool value");

		toml_node *node = new toml_node;
		node->setBool(value);

		return (node);
	}

	toml_node *TOMLParser::parseNil(void)
	{
		toml_node *node = new toml_node;
		/* std::cerr << "Parsing NIL" << std::endl; */
		node->setNil();
		return (node);
	}

	/* parse tha root ! */
	void TOMLParser::parse(void)
	{
		/* std::cerr << "Parsing ROOT" << std::endl; */
		root = new TOMLMap;
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
					/* std::cerr << "MAP_DECL value: " << current.value << std::endl; */
					tokenizer.set_last(NEWLINE);
					tokenizer.rollBackToken();
					processMap();
					continue;
				}
				else
				{
					std::string key = current.value;
					/* std::cerr << key << std::endl; */
					if (tokenizer.getToken().type != ASSIGN)
						throw std::logic_error("EXPECTED assign!");
					current = tokenizer.getToken();
					switch (current.type)
					{
						case STRING:
							{
								tokenizer.rollBackToken();
								(*root)[key] = parseString();
								break;
							}
						case OPEN_BRACKET:
							{
								(*root)[key] = parseArray();
								break;
							}
						case NUMBER:
							{
								tokenizer.rollBackToken();
								(*root)[key] = parseNumber();
								break;
							}
						case BOOL:
							{
								tokenizer.rollBackToken();
								(*root)[key] = parseBool();
								break;
							}
						default:
							{
								std::cerr << "Unknown token, marking as complete" << std::endl;
								completed = true;
								break;
							}
					}
					if (tokenizer.hasMoreTokens())
						current = tokenizer.getToken();
					else
						break;
					if (current.type != NEWLINE)
					{
						throw std::logic_error("EXPECTED newline");
					}
				}
			}
			else
			{
				completed = true;
				break;
			}
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

		/* for (size_t i = 0; i < full_name.size(); i++) */
		/* 	std::cout << full_name[i] << ", "; */
		/* std::cout << std::endl; */

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
						throw std::logic_error("map already in subtable!");
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
