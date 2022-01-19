#ifndef TOMLPARSER_HPP
#define TOMLPARSER_HPP

#include "TOMLNode.hpp"
#include "Tokenizer.hpp"
#include <string>

namespace config
{
	class TOMLParser
	{
		private:
			std::fstream file;
			TOMLMap *root; //root of TOML tree
			/* toml_node *current; //node currently being parsed */
			Tokenizer tokenizer;

		public:
			TOMLParser(const std::string filename) : tokenizer(filename) {}
			TOMLMap *parse(void);

			toml_node *parseMap(void)
			{
				std::cerr << "Parsing object" << std::endl;
				toml_node *node = new toml_node;
				TOMLMap *mapObject = new TOMLMap;
				bool completed = false;
				while (!completed)
				{
					if (tokenizer.hasMoreTokens())
					{
						s_token nextToken;
						try { nextToken = tokenizer.getToken(); }
						catch (std::logic_error e)
						{
							std::cerr << e.what() << std::endl;
							break;
						}
						if (nextToken.type == MAPARRAY_DECL)
						{
							tokenizer.rollBackToken();
							break;
						}
						std::string key = nextToken.value;
						std::cerr << key << std::endl;
						if (tokenizer.getToken().type != ASSIGN)
							throw std::logic_error("EXPECTED assign!");
						nextToken = tokenizer.getToken();
						switch (nextToken.type)
						{
							case STRING:
							{
								tokenizer.rollBackToken();
								(*mapObject)[key] = parseString();
								break;
							}
							case ARR_OPEN:
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
								std::cerr << "reached MAPARRAY_DECL in parseMap" << std::endl;
								completed = true;
								break;
							}
							default:
							{
								/* throw std::logic_error("jopa in parseMap"); */
								std::cerr << "Unknown token, marking as complete" << std::endl;
								completed = true;
								break;
							}
						}
						if (tokenizer.hasMoreTokens())
							nextToken = tokenizer.getToken();
						else
							break;
						if (nextToken.type != NEWLINE)
						{
							throw std::logic_error("EXPECTED newline");
						}
					}
					else
					{
						throw std::logic_error("parseMap: no more tokens");
					}
				}
				node->setObject(mapObject);
				return (node);
			}

			toml_node *parseString(void)
			{
				/* toml_node *node; */
				toml_node *node = new toml_node;
				std::string *sValue;

				std::cerr << "Parsing string" << std::endl;
				s_token token = tokenizer.getToken();
				sValue = new std::string(token.value);
				node->setString(sValue);

				return (node);
			}

			toml_node *parseNumber(void)
			{
				toml_node *node = new toml_node;
				int value;

				std::cerr << "Parsing number" << std::endl;
				s_token token = tokenizer.getToken();
				value = std::atoi(token.value.c_str());
				node->setNumber(value);

				return (node);
			}

			toml_node *parseArray(void)
			{
				std::cerr << "Parsing array" << std::endl;
				toml_node *node;
				toml_node *result = new toml_node;
				TOMLArray *array = new TOMLArray;
				bool completed = false;
				s_token current;

				while (!completed)
				{
					if (!tokenizer.hasMoreTokens())
						throw std::logic_error("No more tokens");
					else
					{
						current = tokenizer.getToken();
						switch (current.type)
						{
							case ARR_OPEN:
							{
								node = parseArray();
								break;
							}
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
								throw std::logic_error("unkown token in parseArray");
							}

						}
						array->push_back(node);
						current = tokenizer.getToken();
						if (current.type == COMMA)
							continue;
						else if (current.type == ARR_CLOSE)
							completed = true;
						else
							throw std::invalid_argument("Unexpected token in array!");
					}
				}
				result->setArr(array);
				return (result);
			}

			toml_node *parseBool(void)
			{
				toml_node *node = new toml_node;
				bool value;

				std::cerr << "Parsing bool" << std::endl;
				s_token token = tokenizer.getToken();
				if (token.value == "true")
					value = true;
				else if (token.value == "false")
					value = false;
				else
					throw std::invalid_argument("Unexpected bool value");
				node->setBool(value);

				return (node);
			}

			toml_node *parseNil(void)
			{
				toml_node *node = new toml_node;
				std::cerr << "Parsing NIL" << std::endl;
				node->setNil();
				return (node);
			}
	};

	/* parse tha root ! */
	TOMLMap *TOMLParser::parse(void)
	{
		std::cerr << "Parsing ROOT" << std::endl;
		root = new TOMLMap;
		bool completed = false;
		while (!completed)
		{
			if (tokenizer.hasMoreTokens())
			{
				s_token current;
				try { current = tokenizer.getToken(); }
				catch (std::logic_error e)
				{
					std::cerr << e.what() << std::endl;
					break;
				}
				if (current.type == MAPARRAY_DECL)
				{
					/* parseMapArray(); */
					tokenizer.rollBackToken();
					std::cerr << "Parsing MapArray" << std::endl;
					toml_node *map_node;
					toml_node *maparr_node;
					s_token current;

					try { current = tokenizer.getToken(); }
					catch (std::logic_error e)
					{
						std::cerr << e.what() << std::endl;
						break;
					}
					if (current.type == MAPARRAY_DECL)
					{
						if (tokenizer.getToken().type != NEWLINE)
							throw std::logic_error("no newline after map_array declaration");
						map_node = parseMap();
					}
					else
						throw std::logic_error("unexpected token in parseMapArray");

					TOMLMap::iterator it;
					std::cout << current.value << std::endl;
					std::string name = current.value;
					std::vector<std::string> full_name;
					size_t dot;

					while (1)
					{
						dot = name.find('.');
						if (dot == std::string::npos)
							break;
						/* std::cout << dot << std::endl; */
						full_name.push_back(name.substr(0, dot));
						name.erase(0, dot + 1);
					}
					full_name.push_back(name);

					for (size_t i = 0; i < full_name.size(); i++)
						std::cout << full_name[i] << ", ";
					std::cout << std::endl;
					
					/* throw std::logic_error("tha end"); */
					TOMLMap *local_root;

					std::vector<std::string>::iterator subname = full_name.begin();
					local_root = root;
					while (1)
					{
						if (subname + 1 == full_name.end())
						{
							it = local_root->find(*subname);
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
							break;
						}
						else
						{
							it = local_root->find(*subname);

							toml_node *map_node2;
							map_node2 = new toml_node;
							TOMLMap *map = new TOMLMap;
							map_node2->setObject(map);
							/* subname not found in local_root */
							if (it == local_root->end())
							{
								(*local_root)[*subname] = map_node2;
								local_root = map;
							}
							/* subname found in local_root */
							else
							{
								local_root = *((it->second)->getMapArray()->end() - 1);
							}

						}
						++subname;
					}

				}
				else
				{
					std::string key = current.value;
					std::cerr << key << std::endl;
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
						case ARR_OPEN:
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
								/* throw std::logic_error("jopa in parseMap"); */
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
		return (root);
	}
}
#endif
