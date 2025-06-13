#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <memory> 

#include "StringUtils.h"

using namespace utils;

//! WARNING: THIS HEADER FILE IS NOT FULLY CROSS-PLATFORM

//! DEFINE PATH SERARTOR BY COMPILATION ON DIFFERENT PLATFORMS
#ifdef _WIN32
    #define PATH_SEPARATOR "\\"
#else
    #define PATH_SEPARATOR "/"
#endif

enum OpenMode {
	READ = std::fstream::in,
	WRITE = std::fstream::out,
	APPEND = std::fstream::app,
	READ_WRITE = std::fstream::in | std::fstream::out,
	BINARY = std::fstream::binary,
	END = std::fstream::end,
	TRUNC = std::fstream::trunc,
};

class FileManager {
	private:
		
		std::map<std::string, std::shared_ptr<std::fstream>> openedFiles;

		//! PRIVATE CONSTRUCTOR IN CASE OF LOGGING
		explicit FileManager() {}

	public:

		void writeToFile(const std::string& absFilePath, const std::string& content) {
			auto& fileStream = getStream(absFilePath, OpenMode::WRITE);
			*fileStream << content;
		}

		std::string getFileContent(const std::string& absFilePath) {
			auto fileStream = getStream(absFilePath, OpenMode::READ);
			std::string line;
			std::string fileContent;

			while (getline(*fileStream, line)) {
				fileContent.append(line);
			}
			return fileContent;
		}

		std::vector<std::string> getFileLines(const std::string& absFilePath) {
			std::ifstream file(absFilePath);
			std::string line;
			std::vector<std::string> fileContent;

			while (getline(file, line)) {
				fileContent.emplace_back(line);
			}

			return fileContent;
		}

		static FileManager& getInstance() {
			static FileManager instance;
			return instance;
		}
	
		~FileManager() {
			
		}

		std::shared_ptr<std::fstream> getStream(const std::string& absFilePath, const OpenMode mode) {
			std::string fileName = StringUtils::lastIndexOf(absFilePath, PATH_SEPARATOR);
			if (!isStreamOpen(fileName)) {
				openFile(absFilePath, mode);
			}
			return openedFiles.at(fileName); 
		}

	private:

		void openFile(const std::string& absFilePath, const OpenMode mode) {
			std::string fileName = StringUtils::lastIndexOf(absFilePath, PATH_SEPARATOR);
			openedFiles[fileName] = std::make_shared<std::fstream>(absFilePath, mode);
		}

		void closeFile(std::shared_ptr<std::fstream>& fileStream) {
			fileStream->close();  
		}

		bool isStreamOpen(const std::string& fileName) {
			return openedFiles.find(fileName) != openedFiles.end() && openedFiles[fileName]->is_open();
		}
		
};

#endif //FILE_MANAGER_H
