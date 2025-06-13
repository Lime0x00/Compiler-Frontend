#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>

namespace utils {
	class StringUtils {
		public:
			std::string trim(const std::string& str) {
				auto start = str.find_first_not_of(" \t\n\r\f\v");
				if (start == std::string::npos)
					return "";
				auto end = str.find_last_not_of(" \t\n\r\f\v");

				return str.substr(start, end - start + 1);
			}


			size_t static find (const std::string& str, const std::string& value) {
				return str.find(value);
			}

			std::string static lastIndexOf(const std::string& str, const std::string& value) {
				size_t index = str.find_last_of(value);
				return str.substr(index + 1);
			}


			 static std::pair<std::string, std::string>  splitStr(const std::string& str, const std::string& separator) {
				std::pair<std::string, std::string> parts;
				size_t pos = str.find(separator);
				if (pos != std::string::npos) {
					parts.first = str.substr(0, pos);
					parts.second = str.substr(pos + separator.length());
				}

				size_t start1 = parts.first.find_first_not_of(" \t");
				size_t end1 = parts.first.find_last_not_of(" \t");
				if (start1 != std::string::npos && end1 != std::string::npos)
					parts.first = parts.first.substr(start1, end1 - start1 + 1);

				size_t start2 = parts.second.find_first_not_of(" \t");
				size_t end2 = parts.second.find_last_not_of(" \t");
				if (start2 != std::string::npos && end2 != std::string::npos)
					parts.second = parts.second.substr(start2, end2 - start2 + 1);

				return parts;
			}

			static bool startsWith(const std::string& str, const std::string& prefix) {
				return str.substr(0, prefix.length()) == prefix;
			}
	};

};
#endif //STRING_UTILS_H