#pragma once

#include <string>

#include "Date.h"

namespace Spacious {
	struct Reminder {
		enum class RecurringUnit {
			DAYS, MONTHS, YEARS
		};
		
		int id;
		std::wstring name;
		bool isRecurring;
		Date startDate{1, 1, 2022};
		int recurringAmount;
		RecurringUnit recurringUnit;
		std::wstring notificationText;
		
		static Date getNextNotification(
			bool isRecurring, const Date &startDate, int recurringAmount, RecurringUnit recurringUnit,
			const Date &today
		);

		bool hasNotification(const Date &previousDate, const Date &date) const;
	};
}