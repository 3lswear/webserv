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
							parseMapArray();
							continue;
						}
						else
						{
							/* take key make decision */
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

			toml_node *parseMapArray(void)
			{
				std::cerr << "Parsing MapArray" << std::endl;
				toml_node *node = new toml_node;
				TOMLMapArray *servers = new TOMLMapArray;
				s_token current;

				while (tokenizer.hasMoreTokens())
				{
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
						servers->second->push_back(parseMap()->getMap());
					}
					else
						throw std::logic_error("unexpected token in parseMapArray");
					if (tokenizer.hasMoreTokens())
						current = tokenizer.getToken();
					else
						break;
					if (current.type != NEWLINE)
					{
						throw std::logic_error("EXPECTED newline");
					}
				}
				node->setMapArray(servers);
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

	toml_node *TOMLParser::parse(void)
	{
		std::string key;
		key = "";

		root = parseMap();
		/* root = parseMapArray(); */
		return (root);

		/* while (tokenizer.hasMoreTokens()) */
		/* { */
		/* 	s_token token; */
		/* 	try */
		/* 	{ */
		/* 		token = tokenizer.getToken(); */
		/* 		/1* std::cout << token.to_string() << std::endl; *1/ */
		/* 		switch (token.type) */
		/* 		{ */
		/* 			case KEY: */
		/* 			{ */
		/* 				toml_node *parsedObject = parseString(); */
		/* 				if (!root) */
		/* 					root = parsedObject; */
		/* 			} */

		/* 			case ARR_OPEN: */
		/* 			{ */
		/* 				toml_node *parsedObject = parseMap(); */
		/* 				/1* parsedObject->printNode(0); *1/ */
		/* 				if (!root) */
		/* 					root = parsedObject; */
		/* 			} */
		/* 			break; */
		/* 			case NUMBER: */
		/* 			{ */
		/* 				tokenizer.rollBackToken(); */
		/* 				toml_node *parsedNumber = parseNumber(); */
		/* 				if (!root) */
		/* 					root = parsedNumber; */
		/* 			} */
		/* 			break; */
		/* 			case STRING: */
		/* 			{ */
		/* 				tokenizer.rollBackToken(); */
		/* 				toml_node *parsedString = parseString(); */
		/* 				if (!root) */
		/* 					root = parsedString; */
		/* 			} */
		/* 			break; */
		/* 			case BOOL: */
		/* 			{ */
		/* 				tokenizer.rollBackToken(); */
		/* 				toml_node *parsedBool = parseBool(); */
		/* 				if (!root) */
		/* 					root = parsedBool; */
		/* 			} */
		/* 			break; */
		/* 			case MAPARRAY_DECL: */
		/* 			{ */

		/* 			} */
		/* 			default: */
		/* 			{ */
		/* 				throw std::logic_error("JOPA :("); */
		/* 			} */
		/* 		} */
		/* 	} */
		/* 	catch (std::logic_error err) */
		/* 	{ */
		/* 		break; */
		/* 	} */
		/* } */
		return (root);
	}
}
#endif
