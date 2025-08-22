//
// Created by lime0x001 on 4/15/25.
//

#ifndef PREDICTIVE_TABLE_H
#define PREDICTIVE_TABLE_H

#include <set>
#include <utility>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <iomanip>


#include "Grammar.h"

class PredictiveTable {
    private:
        Grammar grammar;

        std::map<std::string, std::set<std::string>> firstSet;

        std::map<std::string, std::set<std::string>> followSet;

        std::map<std::string, std::map<std::string, std::vector<std::string>>> parseTable;

    public:

        explicit PredictiveTable(Grammar grammar) : grammar(grammar) {
        }

        bool isTerminal(const std::string& symbol) const {
            return grammar.isTerminal(symbol);
        }

        bool isNonTerminal(const std::string& symbol) const {
            return grammar.isNonTerminal(symbol);
        }

        std::string getStartSymbol() const {
            return grammar.getStartSymbol();
        }

        std::map<std::string, std::set<std::string>> getFirstSet() {
            if (firstSet.empty()) {
                computeFirstSet();
            }
            return firstSet;
        }

        std::map<std::string, std::set<std::string>> getFollowSet() {
            if (followSet.empty()) {
                computeFollowSet();
            }
            return followSet;
        }

        std::map<std::string, std::map<std::string, std::vector<std::string>>> getParseTable() {
            if (parseTable.empty()) {
                buildParseTable();
            }
            return parseTable;
        }

        void printFirstSet() {
            if (firstSet.empty()) {
                computeFirstSet();
            }

            std::cout << "\n=== First Sets ===\n";
            std::map<std::string, std::set<std::string>>::iterator it;
            for (it = firstSet.begin(); it != firstSet.end(); ++it) {
                std::cout << it->first << " -> ";
                std::set<std::string>::iterator sit;
				std::cout << "{";
                for (sit = it->second.begin(); sit != it->second.end(); ++sit) {
					std::cout <<*sit << ",";
                }
				std::cout << "}\n";
            }
        }

        void printFollowSet() {
            if (followSet.empty()) {
                computeFollowSet();
            }

            std::cout << "\n=== Follow Sets ===\n";
            std::map<std::string, std::set<std::string>>::iterator it;
            for (it = followSet.begin(); it != followSet.end(); ++it) {
                std::cout << it->first << " -> ";
                std::set<std::string>::iterator sit;
				std::cout << "{";
                for (sit = it->second.begin(); sit != it->second.end(); ++sit) {
                    std::cout << *sit << ",";
                }
				std::cout << "}\n";
            }
        }

        void printParseTable() {
            if (parseTable.empty()) buildParseTable();

            std::cout << "\n=== Predictive Parse Table ===\n";

            std::cout << std::left << std::setw(15) << "Non-Terminal"
                      << std::setw(15) << "Terminal"
                      << "Production\n";
            std::cout << "--------------------------------------------------------\n";

            std::map<std::string, std::map<std::string, std::vector<std::string>>>::iterator it;
            for (it = parseTable.begin(); it != parseTable.end(); ++it) {
                std::map<std::string, std::vector<std::string>>::iterator jt;
                for (jt = it->second.begin(); jt != it->second.end(); ++jt) {
                    std::cout << std::left << std::setw(15) << it->first
                              << std::setw(15) << jt->first
                              << "-> ";
                    std::vector<std::string>::iterator vit;
                    for (vit = jt->second.begin(); vit != jt->second.end(); ++vit) {
                        std::cout << *vit << " ";
                    }
                    std::cout << "\n";
                }
            }
        }

    private:
        void computeFirstSet() {
            std::map<std::string, std::set<std::string>> mem;
            std::map<std::string, std::vector<std::vector<std::string>>> productions = grammar.getProductions();
            std::map<std::string, std::vector<std::vector<std::string>>>::iterator it;
            for (it = productions.begin(); it != productions.end(); ++it) {
                for (size_t i = 0; i < it->second.size(); ++i) {
                    std::set<std::string> first = FIRST(it->second[i], mem);
                    firstSet[it->first].insert(first.begin(), first.end());
                }
            }
        }

        std::set<std::string> FIRST(const std::string& symbol, std::map<std::string, std::set<std::string>>& mem) const {
            if (grammar.isTerminal(symbol)) {
				std::set<std::string> result;
				result.insert(symbol);
				return result;
			}

            if (mem.find(symbol) != mem.end()) return mem[symbol];

            std::set<std::string> result;
            std::vector<std::vector<std::string>> productions = grammar.getProduction(symbol);
            for (size_t i = 0; i < productions.size(); ++i) {
                std::set<std::string> firstOfRhs = FIRST(productions[i], mem);
                result.insert(firstOfRhs.begin(), firstOfRhs.end());
            }

            mem[symbol] = result;
            return result;
        }

        std::set<std::string> FIRST(const std::vector<std::string>& rhs, std::map<std::string, std::set<std::string>>& mem) const {
            std::set<std::string> result;
            for (size_t i = 0; i < rhs.size(); ++i) {
                std::set<std::string> first = FIRST(rhs[i], mem);
                result.insert(first.begin(), first.end());

                if (first.find("~") == first.end()) break;
            }
            return result;
        }

		void computeFollowSet() {
			if (firstSet.empty()) {
				computeFirstSet();
			}

			std::map<std::string, std::set<std::string>> mem;

			std::map<std::string, std::vector<std::vector<std::string>>> productions = grammar.getProductions();
    
			std::map<std::string, std::vector<std::vector<std::string>>>::iterator it;
			for (it = productions.begin(); it != productions.end(); ++it) {
				const std::string& nonTerminal = it->first;
				if (followSet.find(nonTerminal) == followSet.end()) {
					followSet[nonTerminal] = std::set<std::string>();
				}
			}

			const std::string& startSymbol = productions.begin()->first;
			followSet[startSymbol].insert("$");

			bool changed = true;
			while (changed) {
				changed = false;

				for (it = productions.begin(); it != productions.end(); ++it) {
					for (size_t i = 0; i < it->second.size(); ++i) {
						if (FOLLOW(it->first, it->second[i], mem)) {
							changed = true;
						}
					}
				}
			}
		}



        bool FOLLOW(const std::string& lhs, const std::vector<std::string>& rhs, std::map<std::string, std::set<std::string>>& mem) {
            bool changed = false;

            for (size_t i = 0; i < rhs.size(); ++i) {
                const std::string& B = rhs[i];
                if (!grammar.isNonTerminal(B)) continue;

                std::set<std::string> original = followSet[B];

                if (i + 1 < rhs.size()) {
                    std::vector<std::string> beta(rhs.begin() + i + 1, rhs.end());
                    std::set<std::string> firstOfBeta = FIRST(beta, mem);
                    bool hasEpsilon = (firstOfBeta.find("~") != firstOfBeta.end());
                    firstOfBeta.erase("~");

                    followSet[B].insert(firstOfBeta.begin(), firstOfBeta.end());

                    if (hasEpsilon) {
                        followSet[B].insert(followSet[lhs].begin(), followSet[lhs].end());
                    }
                } else {
                    followSet[B].insert(followSet[lhs].begin(), followSet[lhs].end());
                }

                if (followSet[B].size() > original.size()) {
                    changed = true;
                }
            }

            return changed;
        }

        void buildParseTable() {
            if (followSet.empty()) computeFollowSet();

            std::map<std::string, std::set<std::string>> mem;

            std::map<std::string, std::vector<std::vector<std::string>>> productions = grammar.getProductions();
            std::map<std::string, std::vector<std::vector<std::string>>>::iterator it;

            for (it = productions.begin(); it != productions.end(); ++it) {
                for (size_t i = 0; i < it->second.size(); ++i) {
                    std::set<std::string> first = FIRST(it->second[i], mem);

                    std::set<std::string>::iterator fit;
                    for (fit = first.begin(); fit != first.end(); ++fit) {
                        if (*fit != "~") {
                            parseTable[it->first][*fit] = it->second[i];
                        }
                    }

                    if (first.find("~") != first.end()) {
                        std::set<std::string>::iterator fset;
                        for (fset = followSet[it->first].begin(); fset != followSet[it->first].end(); ++fset) {
                            parseTable[it->first][*fset] = it->second[i];
                        }
                    }
                }
            }
        }

};

#endif //PREDICTIVE_TABLE_H
