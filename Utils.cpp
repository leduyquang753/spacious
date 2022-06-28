#include "pch.h"

#include <filesystem>
#include <string>

namespace Spacious {
	namespace Utils {
		std::filesystem::path getDataFolderPath() {
			using namespace std::string_literals;
			
			std::wstring appDataPathString(
				GetEnvironmentVariable(L"LocalAppData", nullptr, 0), 0
			);
			GetEnvironmentVariable(
				L"LocalAppData", appDataPathString.data(),
				static_cast<DWORD>(appDataPathString.size())
			);
			appDataPathString.pop_back(); // Get rid of the null terminator.
			std::filesystem::path path = appDataPathString;
			path /= "Spacious"s;
			return path;
		}
	}
}