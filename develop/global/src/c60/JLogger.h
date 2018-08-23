/**
 * Класс для доступа к java журналу
 */
#pragma once

#include "dgn/LoggerSetup.hpp"
class JLogger: dgn::LoggerSetup{
public:
		/**Сообщение для отладки**/
		static void info(const wchar_t* fmt, ...);

		/**Сообщение о проблеме**/
		static void warn(const wchar_t* fmt, ...);

		/**Сообщение об ошибке**/
		static void error(const wchar_t *file, const unsigned int line, const wchar_t* fmt, ...);
};

#define WIDE2(x) L##x
#define WIDE1(x) WIDE2(x)
#define WFILE WIDE1(__FILE__)
#define ERROR(fmt, args ...) JLogger::error(WFILE, __LINE__, fmt, args);
