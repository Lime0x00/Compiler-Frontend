//
// Created by lime0x001 on 4/15/25.
//

#ifndef LL1_PARSER_H
#define LL1_PARSER_H

#include <stack>
#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "PredictiveTable.h"

class LL1Parser {
	private:
		PredictiveTable table;
		std::vector<std::string> tokens;

	public:
		LL1Parser(const std::vector<std::string>& tokensInput, const PredictiveTable& tableInput)
		: table(tableInput), tokens(tokensInput) {}

		bool parse() {
			std::stack<std::string> parseStack;
			parseStack.push("$");
			parseStack.push(table.getStartSymbol());

			size_t i = 0;
			tokens.push_back("$");

			const std::map<std::string, std::map<std::string, std::vector<std::string> > >& parseTable = table.getParseTable();

			while (!parseStack.empty()) {
				std::string top = parseStack.top();
				std::string currentToken = tokens.at(i);
				parseStack.pop();

				if (top == currentToken) {
					i++;
				}
				else if (table.isTerminal(top)) {
					std::cerr << "Error: unexpected token \"" << currentToken << "\" at position " << i << ", expected \"" << top << "\"\n";
					return false;
				}
				else {
					std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator row = parseTable.find(top);
					if (row == parseTable.end()) {
						std::cerr << "Error: No entry for non-terminal '" << top << "' in the parse table.\n";
						return false;
					}

					std::map<std::string, std::vector<std::string>>::const_iterator it = row->second.find(currentToken);
					if (it == row->second.end()) {
						std::cerr << "Error: no rule for (" << top << ", " << currentToken << ")\n";
						return false;
					}

					const std::vector<std::string>& production = it->second;
					for (int j = static_cast<int>(production.size()) - 1; j >= 0; --j) {
						if (production[j] != "~") {
							parseStack.push(production[j]);
						}
					}
				}
			}

			return i == tokens.size();
		}

	private:
		std::map<std::string, std::vector<std::string> >::iterator getRule(const std::string& lhs, const std::string& input) {
			return table.getParseTable().at(lhs).find(input);
		}

		std::map<std::string, std::vector<std::string> >::iterator getEnd(const std::string& nonTerminal) {
			return table.getParseTable().at(nonTerminal).end();
		}
};

#endif // LL1_PARSER_H

