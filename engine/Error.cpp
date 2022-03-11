#include "pch.h"
#include <SWE/Engine/error.h>

namespace swe
{
	Error::Error(std::string m, errorLevel l, std::source_location sl)
		: message(m), level(l)
	{
#ifdef _DEBUG
		uint32_t line = sl.line();
		const char* file = sl.file_name();
		const char* func = sl.function_name();

		switch (l)
		{
		case errorLevel::Info:
			printf("%s (%s)\n", m.c_str(), func);
			break;
		case errorLevel::Warning:
#ifdef SWE_SHOW_WARNINGS
			printf("Warning in file %s on line %u.\n\t-> \"%s\"\n", file, line, m.c_str());
#else
			printf("Warnings supressed.%s", "\n");
#endif
			break;
		case errorLevel::Error:
			printf("Error in file %s on line %u.\n\t-> \"%s\"\n", file, line, m.c_str());
			break;
		}
#endif
	}
}