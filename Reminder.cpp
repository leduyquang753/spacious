#include "Date.h"

#include "Reminder.h"

namespace Spacious {
	bool Reminder::hasNotification(const Date &date) const {
		if (isRecurring) {
			int difference = 0;
			switch (recurringUnit) {
				case RecurringUnit::DAYS:
					difference = date.index() - startDate.index();
					break;
				case RecurringUnit::MONTHS:
					if (date.day() != startDate.day()) return false;
					difference = (date.year() - startDate.year())*12 + date.month() - startDate.month();
					break;
				case RecurringUnit::YEARS:
					if (date.day() != startDate.day() || date.month() != startDate.month()) return false;
					difference = date.year() - startDate.year();
					break;
			}
			return difference >= 0 && difference % recurringAmount == 0;
		} else {
			return date.index() == startDate.index();
		}
	}
}