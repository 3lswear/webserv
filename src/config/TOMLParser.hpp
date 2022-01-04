#include "TOMLNode.hpp"
#include "Tokenizer.hpp"

namespace config
{
	class TOMLParser
	{
		private:
			std::fstream file;
			toml_node *root; //root of TOML tree
			toml_node *current; //node currently being parsed
			Tokenizer tokenizer;

		public:
			TOMLParser(const std::string filename) : tokenizer(filename) {}
			void parse(void);

			toml_node *parseObject(void);
			toml_node *parseString(void);
			toml_node *parseNumber(void);
			toml_node *parseArray(void);
			toml_node *parseBool(void);
			toml_node *parseNil(void);
	};

	void TOMLParser::parse(void)
	{
		std::string key;
		key = "";

		while (tokenizer.hasMoreTokens())
		{
			s_token token;
			try
			{
				token = tokenizer.getToken();
				std::cout << token.to_string() << std::endl;
				switch (token.type)
				{
					case ARR_OPEN:
						{
							toml_node *parsedObject = parseObject();
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
					case STRING:
						{
							tokenizer.rollBackToken();
							toml_node *parsedString = parseString();
							if (!root)
								root = parsedString;
						}
					case BOOL:
						{
							tokenizer.rollBackToken();
							toml_node *parsedBool = parseBool();
							if (!root)
								root = parsedBool;
						}
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
	}

	toml_node *TOMLParser::parseString(void)
	{
		toml_node *node;
		std::string *sValue;

		std::cout << "Parsing string" << std::endl;
		s_token token = tokenizer.getToken();
		sValue = new std::string(token.value);
		node->setString(sValue);

	}
}
