//
// Created by lime0x001 on 4/15/25.
//
#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <map>
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>

#include "FileManager.h"
#include "StringUtils.h"
#include "Helpers.h"

using namespace utils;


class Grammar {
	private:
		std::string grammar;

		std::map<std::string, std::vector<std::vector<std::string>>> productions;

		std::set<std::string> terminals;

		std::set<std::string> nonTerminals;

		std::string startSymbol;

	public:
		explicit Grammar (const std::string& absFilePath, const bool isFile = false) {
			auto fileStream = FileManager::getInstance().getStream(absFilePath, OpenMode::READ);
			this->grammar = (isFile) ? FileManager::getInstance().getFileContent(grammar) : grammar;
			processGrammar(fileStream);
		}

		bool isTerminal(const std::string& symbol) const {
			return terminals.find(symbol) != terminals.end();
		}

		bool isNonTerminal(const std::string& symbol) const {
			return nonTerminals.find(symbol) != nonTerminals.end();
		}

		std::set<std::string> getTerminals() const {
			return terminals;
		}

		std::set<std::string> getNonTerminals() const {
			return nonTerminals;
		}

		std::map<std::string, std::vector<std::vector<std::string>>> getProductions() const {
			return productions;
		}

		std::vector<std::vector<std::string>> getProduction(const std::string& lhs) const {
			return productions.at(lhs);
		}

		std::string getStartSymbol() const {
			return startSymbol;
		}

		void printGrammar() const {
			std::cout << "\n=== Grammar ===\n";

			std::cout << "\nTerminals:\n";
			printTerminals();

			std::cout << "\nNon-Terminals:\n";
			printNonTerminals();

			std::cout << "\nProductions:\n";
			printProductions();
		}

		void printTerminals() const {
			if (terminals.empty()) {
				std::cout << "No terminals available.\n";
				return;
			}

			for (std::set<std::string>::const_iterator it = terminals.begin(); it != terminals.end(); ++it) {
				std::cout << *it << "\n";
			}
		}

		void printNonTerminals() const {
			if (nonTerminals.empty()) {
				std::cout << "No non-terminals available.\n";
				return;
			}

			for (std::set<std::string>::const_iterator it = nonTerminals.begin(); it != nonTerminals.end(); ++it) {
				std::cout << *it << "\n";
			}
		}

		void printProductions() const {
			if (productions.empty()) {
				std::cout << "No productions available.\n";
				return;
			}

			for (std::map<std::string, std::vector<std::vector<std::string>>>::const_iterator it = productions.begin(); it != productions.end(); ++it) {
				const std::string& lhs = it->first;
				const std::vector<std::vector<std::string>>& rhsList = it->second;

				std::cout << lhs << " ::= ";
				for (size_t i = 0; i < rhsList.size(); ++i) {
					const std::vector<std::string>& symbols = rhsList[i];
					for (size_t j = 0; j < symbols.size(); ++j) {
						std::cout << symbols[j] << ' ';
					}
					if (i != rhsList.size() - 1) {
						std::cout << "| ";
					}
				}
				std::cout << "\n";
			}
		}

	private:
		void processGrammar(const std::shared_ptr<std::fstream>& fileStream) {
			std::string line;

			if (!fileStream) {
				std::cerr << "Unable to open file!" << std::endl;
				return;
			}

			bool isFirstProduction = true;

			while (std::getline(*fileStream, line)) {
				if (StringUtils::startsWith(line, "//") || line.empty()) {
					continue;
				}
				std::pair<std::string, std::string> parts = StringUtils::splitStr(line, "::=");
				std::string lhs = parseLHS(parts.first);
				std::vector<std::vector<std::string>> rhs = parseRHS(parts.second);
				productions.insert(std::make_pair(lhs, rhs));
				if (isFirstProduction) {
					startSymbol = lhs;
					isFirstProduction = false;
				}
			}
		}

		std::string parseLHS(const std::string& lhs) {
			std::string nonTerminal = parseNonTerminal(lhs);
			nonTerminals.insert(nonTerminal);
			return nonTerminal;
		}

		std::vector<std::vector<std::string>> parseRHS(const std::string& rhs) {
			std::vector<std::vector<std::string>> rule;
			std::vector<std::string> current;

			for (size_t i = 0; i < rhs.length(); ) {
				if (Helpers::isSpace(rhs.at(i))) {
					i++;
					continue;
				}

				if (rhs[i] == '|') {
					if (!current.empty()) {
						rule.push_back(current);
						current.clear();
					}
					i++;
					continue;
				}

				if (rhs[i] == '<') {
					std::string nonTerminal = parseNonTerminal(rhs.substr(i));

					if (nonTerminal.empty()) {
						std::cerr << "Error: Failed to parse non-terminal at position " << i << std::endl;
						return std::vector<std::vector<std::string>>();
					}

					current.push_back(nonTerminal);
					i += nonTerminal.length() + 2;
				} else {
					std::string terminal = parseTerminal(rhs.substr(i));

					if (terminal.empty()) {
						std::cerr << "Error: Failed to parse terminal at position " << i << std::endl;
						return std::vector<std::vector<std::string>>();
					}

					if (StringUtils::startsWith(terminal, "\\")) {
						terminal = terminal.substr(1);
						i++;
					} 
					
					current.push_back(terminal);
					terminals.insert(terminal);
					i += terminal.length();
				}
			}

			if (!current.empty()) {
				rule.push_back(current);
			}

			return rule;
		}

		static std::string parseNonTerminal(const std::string& str) {
			for (size_t i = 0; i < str.length(); i++) {
				if (str[i] == '<') {
					std::string nonTerminal;
					i++;
					while (i < str.length() && str[i] != '>') {
						nonTerminal += str[i];
						i++;
					}

					if (i >= str.length() || str[i] != '>') {
						std::cout << "Error: Cannot Parse Non-Terminal" << std::endl;
						return "";
					}
					return nonTerminal;
				}
			}
			return "";
		}

		static std::string parseTerminal(const std::string& str) {
			std::string terminal;
		
			for (size_t i = 0; i < str.length(); i++) {
				if (isspace(str.at(i))) {
					return terminal;
				} else if (str.at(i) == '\\') {
					terminal += str.at(i);
					while (i < str.length() - 1 && !(str.at(i++))) {
						terminal += str.at(i);
					}
					return terminal;
				}
				terminal += str.at(i);
			}
			return terminal;
		}
};

#endif //GRAMMAR_H
