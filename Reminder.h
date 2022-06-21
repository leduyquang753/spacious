#pragma once

#include <string>

namespace Spacious {
	struct Reminder {
		enum class RecurringUnit {
			DAYS, MONTHS, YEARS
		};
		
		int id;
		std::wstring name;
		bool isRecurring;
		int startDate;
		int recurringAmount;
		RecurringUnit recurringUnit;
		std::wstring notificationText;
	};
}