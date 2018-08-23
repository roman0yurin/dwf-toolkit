//
// Created by den on 14.08.18.
//

#include "JLogger.h"
#include <cstdarg>
#include <cstring>
#include "dgn/JLogger.hpp"

static std::shared_ptr<dgn::JLogger> logger;
#define MAX_MESSAGE_LEN 1024
static wchar_t message_buffer[MAX_MESSAGE_LEN + 1];

/**Установка объекта для доступа к java журналу из C++ */
void dgn::LoggerSetup::setupLogger(const std::shared_ptr<dgn::JLogger> & loggerRef){
	logger = loggerRef;
	memset(message_buffer, 0, sizeof(message_buffer));
}

void JLogger::info(const wchar_t *fmt, ...) {
	va_list argList;

	va_start(argList, fmt);
	int len = vswprintf(message_buffer, MAX_MESSAGE_LEN, fmt, argList);
	va_end(argList);
	std::wstring str(message_buffer, len);
	logger->info(str);
}

void JLogger::warn(const wchar_t *fmt, ...) {
	va_list argList;

	va_start(argList, fmt);
	int len = vswprintf(message_buffer, MAX_MESSAGE_LEN, fmt, argList);
	va_end(argList);
	std::wstring str(message_buffer, len);
	logger->warn(str);
}


void JLogger::error(const wchar_t *file, const unsigned int line, const wchar_t *fmt, ...) {
	va_list argList;
	int prefixLen = swprintf(message_buffer, MAX_MESSAGE_LEN, L"C++ ERROR: ");

	va_start(argList, fmt);
	int len = vswprintf(&message_buffer[prefixLen], MAX_MESSAGE_LEN - prefixLen, fmt, argList) + prefixLen;
	va_end(argList);

	len += swprintf(&message_buffer[len], MAX_MESSAGE_LEN - len, L" At %S line %d", file, line);
	std::wstring str(message_buffer, len);
	logger->error(str);
}


