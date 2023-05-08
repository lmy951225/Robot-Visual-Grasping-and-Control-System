#ifndef AMC_CORE_LOG_H_
#define AMC_CORE_LOG_H_

#include <string>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <exception>

#include <object.hpp>

#define LOG_DEBUG amc::core::log() \
	<< std::setw(amc::core::LOG_TYPE_WIDTH) << "DEBUG" << "|" \
	<< std::setw(amc::core::LOG_TIME_WIDTH) << amc::core::logFileTimeFormat(std::chrono::system_clock::now()) <<"|" \
	<< std::setw(amc::core::LOG_FILE_WIDTH) << std::string(__FILE__).substr(std::string(__FILE__).find_last_of("/\\") + 1) <<"|"\
	<< std::setw(amc::core::LOG_LINE_WIDTH) << __LINE__ <<"|"

#define LOG_INFO amc::core::log() \
	<< std::setw(amc::core::LOG_TYPE_WIDTH) << "INFO" << "|" \
	<< std::setw(amc::core::LOG_TIME_WIDTH) << amc::core::logFileTimeFormat(std::chrono::system_clock::now()) <<"|" \
	<< std::setw(amc::core::LOG_FILE_WIDTH) << std::string(__FILE__).substr(std::string(__FILE__).find_last_of("/\\") + 1) <<"|"\
	<< std::setw(amc::core::LOG_LINE_WIDTH) << __LINE__ <<"|"

#define LOG_ERROR amc::core::log() \
	<< std::setw(amc::core::LOG_TYPE_WIDTH) << "ERROR" << "|" \
	<< std::setw(amc::core::LOG_TIME_WIDTH) << amc::core::logFileTimeFormat(std::chrono::system_clock::now()) <<"|" \
	<< std::setw(amc::core::LOG_FILE_WIDTH) << std::string(__FILE__).substr(std::string(__FILE__).find_last_of("/\\") + 1) <<"|"\
	<< std::setw(amc::core::LOG_LINE_WIDTH) << __LINE__ <<"|"

#define LOG_FATAL amc::core::log() \
	<< std::setw(amc::core::LOG_TYPE_WIDTH) << "FATAL" << "|" \
	<< std::setw(amc::core::LOG_TIME_WIDTH) << amc::core::logFileTimeFormat(std::chrono::system_clock::now()) <<"|" \
	<< std::setw(amc::core::LOG_FILE_WIDTH) << std::string(__FILE__).substr(std::string(__FILE__).find_last_of("/\\") + 1) <<"|"\
	<< std::setw(amc::core::LOG_LINE_WIDTH) << __LINE__ <<"|"

#define LOG_CONTINUE amc::core::log() \
	<< std::setw(amc::core::LOG_SPACE_WIDTH) << "|"

#define LOG_DEBUG_EVERY_N(n) static thread_local int LOG_OCCURRENCES_MOD_N ## __LINE__ = 0; \
	if (++LOG_OCCURRENCES_MOD_N ## __LINE__ > n) LOG_OCCURRENCES_MOD_N ## __LINE__ -= n; \
	if (LOG_OCCURRENCES_MOD_N ## __LINE__ == 1) LOG_DEBUG

#define LOG_INFO_EVERY_N(n) static thread_local int LOG_OCCURRENCES_MOD_N ## __LINE__ = 0; \
	if (++LOG_OCCURRENCES_MOD_N ## __LINE__ > n) LOG_OCCURRENCES_MOD_N ## __LINE__ -= n; \
	if (LOG_OCCURRENCES_MOD_N ## __LINE__ == 1) LOG_INFO

#define LOG_ERROR_EVERY_N(n) static thread_local int LOG_OCCURRENCES_MOD_N ## __LINE__ = 0; \
	if (++LOG_OCCURRENCES_MOD_N ## __LINE__ > n) LOG_OCCURRENCES_MOD_N ## __LINE__ -= n; \
	if (LOG_OCCURRENCES_MOD_N ## __LINE__ == 1) LOG_ERROR

#define LOG_FATAL_EVERY_N(n) static thread_local int LOG_OCCURRENCES_MOD_N ## __LINE__ = 0; \
	if (++LOG_OCCURRENCES_MOD_N ## __LINE__ > n) LOG_OCCURRENCES_MOD_N ## __LINE__ -= n; \
	if (LOG_OCCURRENCES_MOD_N ## __LINE__ == 1) LOG_FATAL

#define LOG_AND_THROW(error) LOG_ERROR << error.what() << std::endl, throw error

#define THROW_FILE_AND_LINE(error) throw std::runtime_error(std::string(__FILE__) + "_" + std::to_string(__LINE__)+ ":" + error)

namespace amc::core
{
	enum
	{
		LOG_TYPE_WIDTH = 5,
		LOG_TIME_WIDTH = 20,
		LOG_FILE_WIDTH = 25,
		LOG_LINE_WIDTH = 5,
		LOG_SPACE_WIDTH = LOG_TYPE_WIDTH + 1 + LOG_TIME_WIDTH + 1 + LOG_FILE_WIDTH + 1 + LOG_LINE_WIDTH + 1,
	};

	auto logDirectory(const std::filesystem::path &log_dir_path = std::filesystem::path())->void; 
	auto logFile(const std::filesystem::path &log_file_path = std::filesystem::path())->void;
	auto logStream(std::ostream *s = nullptr)->void;
	auto log()->std::ostream&;

	auto logDirPath()->std::filesystem::path;
	auto logExeName()->std::string;
	auto logFileTimeFormat(const std::chrono::system_clock::time_point &time)->std::string;
}

#endif
