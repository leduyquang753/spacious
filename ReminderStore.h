#pragma once

#include <filesystem>
#include <vector>

#include "Reminder.h"

namespace Spacious {
	struct ReminderStore {
		std::filesystem::path filePath;
		std::vector<Reminder> reminders;
		int nextID = 1;

		void load();
		void save();
	};
}