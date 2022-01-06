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
			toml_node *root; //root of TOML tree
			/* toml_node *current; //node currently being parsed */
			Tokenizer tokenizer;

		public:
			TOMLParser(const std::string filename) : tokenizer(filename) {}
			toml_node *parse(void);

			toml_node *parseMap(void)
			{
				std::cout << "Parsing object" << std::endl;
				toml_node *node = new toml_node;
				TOMLMap *mapObject = new TOMLMap;
				bool completed = false;
				while (!completed)
				{
					if (tokenizer.hasMoreTokens())
					{
						s_token nextToken;
						try
						{
							nextToken = tokenizer.getToken();
						}
						catch (std::logic_error e)
						{
							std::cerr << e.what() << std::endl;
							break;
						}
						std::string key = nextToken.value;
						std::cout << key << std::endl;
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
								(*mapObject)[key] = parseBool();
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
						/* if (tokenizer.getToken().type == NEWLINE) */
						/* 	completed = true; */
						/* else */
						/* 	throw std::logic_error("EXPECTED newline"); */
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

				std::cout << "Parsing string" << std::endl;
				s_token token = tokenizer.getToken();
				sValue = new std::string(token.value);
				node->setString(sValue);

				return (node);
			}

			toml_node *parseNumber(void)
			{
				toml_node *node = new toml_node;
				int value;

				std::cout << "Parsing number" << std::endl;
				s_token token = tokenizer.getToken();
				value = std::atoi(token.value.c_str());
				node->setNumber(value);

				return (node);
			}

			toml_node *parseArray(void)
			{
				std::cout << "Parsing array" << std::endl;
				toml_node *node = new toml_node;
				TOMLArray *array = new TOMLArray;
				bool completed = false;

				while (!completed)
				{
					if (tokenizer.hasMoreTokens())
					{
						throw std::logic_error("No more tokens");
					}
					else
					{
						s_token nextToken = tokenizer.getToken();
						switch (nextToken.type)
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
								throw std::logic_error("unkown token in parseList");
							}

						}
						array->push_back(node);
						nextToken = tokenizer.getToken();
						if (nextToken.type == ARR_CLOSE)
							completed = true;
					}
				}
				node->setArr(array);
				return (node);
			}

			toml_node *parseBool(void)
			{
				toml_node *node = new toml_node;
				bool value;

				std::cerr << "Parsing bool" << std::endl;
				s_token token = tokenizer.getToken();
				if (token.value == "true")
					value = true;
				else
					value = false;
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

	toml_node *TOMLParser::parse(void)
	{
		std::string key;
		key = "";

		root = parseMap();
		return (root);

		while (tokenizer.hasMoreTokens())
		{
			s_token token;
			try
			{
				token = tokenizer.getToken();
				/* std::cout << token.to_string() << std::endl; */
				switch (token.type)
				{
					case KEY:
					{
						toml_node *parsedObject = parseString();
						if (!root)
							root = parsedObject;
					}

					case ARR_OPEN:
					{
						toml_node *parsedObject = parseMap();
						/* parsedObject->printNode(0); */
						if (!root)
							root = parsedObject;
					}
					break;
					case NUMBER:
					{
						tokenizer.rollBackToken();
						toml_node *parsedNumber = parseNumber();
						if (!root)
							root = parsedNumber;
					}
					break;
					case STRING:
					{
						tokenizer.rollBackToken();
						toml_node *parsedString = parseString();
						if (!root)
							root = parsedString;
					}
					break;
					case BOOL:
					{
						tokenizer.rollBackToken();
						toml_node *parsedBool = parseBool();
						if (!root)
							root = parsedBool;
					}
					break;
					default:
					{
						throw std::logic_error("JOPA :(");
					}
				}
			}
			catch (std::logic_error err)
			{
				break;
			}
		}
		return (root);
	}
}
#endif
