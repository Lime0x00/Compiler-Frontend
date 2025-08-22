
#ifndef HELPERS_H
#define HELPERS_H

namespace utils {
	class Helpers {
		public:
			static bool isSpace(const char c) {
				return c == ' '  ||
					   c == '\n' ||
					   c == '\r' ||
 					   c == '\t'
					   ;
			}

			static bool isAldig(const char c) {
				return isAlpha(c) || isDigit(c);
			}

			static bool isAlpha(const char c) {
				return (c >= 'a' && c <= 'z') 
						|| 
					   (c >= 'A' && c <= 'Z');
			}

			static bool isDigit(const char c) {
				return c >= '0' && c <= '9';
			}
	};
};

#endif //HELPERS_H