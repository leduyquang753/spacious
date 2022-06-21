#include "pch.h"

#include <filesystem>
#include <fstream>
#include <ios>
#include <limits>
#include <string>

#include "ReminderStore.h"

namespace Spacious {
	void ReminderStore::load() {
		using namespace std::string_literals;
		
		std::wstring appDataPathString(
			GetEnvironmentVariable(L"LocalAppData", nullptr, 0), 0
		);
		GetEnvironmentVariable(
			L"LocalAppData", appDataPathString.data(),
			static_cast<DWORD>(appDataPathString.size())
		);
		appDataPathString.pop_back(); // Get rid of the null terminator;
		filePath = appDataPathString;
		filePath /= "Spacious"s;
		if (!std::filesystem::exists(filePath))
			std::filesystem::create_directory(filePath);
		filePath /= "Reminders.txt"s;
		if (!std::filesystem::exists(filePath)) {
			save();
		} else {
			std::wifstream file(filePath);
			#define nextLine()\
				file.ignore(std::numeric_limits<std::streamsize>::max(), '\n')
			file >> nextID;
			int count;
			file >> count;
			reminders.reserve(count);
			for (int i = 0; i != count; ++i) {
				auto &reminder = reminders.emplace_back();
				file >> reminder.id;
				nextLine();
				std::getline(file, reminder.name);
				file >> reminder.isRecurring;
				file >> reminder.startDate;
				if (reminder.isRecurring) {
					file >> reminder.recurringAmount;
					int recurringUnitValue;
					file >> recurringUnitValue;
					reminder.recurringUnit = static_cast<::Spacious::Reminder::RecurringUnit>(recurringUnitValue);
				}
				nextLine();
				std::getline(file, reminder.notificationText);
			}
			#undef nextLine
		}
	}

	void ReminderStore::save() {
		std::wofstream file(filePath);
		file
			<< nextID << '\n'
			<< reminders.size() << '\n';
		for (const auto &reminder : reminders) {
			file
				<< reminder.id << '\n'
				<< reminder.name << '\n'
				<< reminder.isRecurring << '\n'
				<< reminder.startDate << '\n';
			if (reminder.isRecurring) {
				file
					<< reminder.recurringAmount << '\n'
					<< static_cast<int>(reminder.recurringUnit) << '\n';
			}
			file << reminder.notificationText << '\n';
		}
	}
}