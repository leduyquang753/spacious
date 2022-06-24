#include "Date.h"

#include "Reminder.h"

namespace Spacious {
	bool Reminder::hasNotification(const Date &previousDate, const Date &date) const {
		if (date.index() < startDate.index()) return false;
		if (isRecurring) {
			int difference = 0;
			switch (recurringUnit) {
				case RecurringUnit::DAYS: {
					const int mod = startDate.index() % recurringAmount;
					return (previousDate.index()-mod) / recurringAmount < (date.index()-mod) / recurringAmount;
				}
				case RecurringUnit::MONTHS: {
					#define monthIndex(d) (d.year()*12 + d.month())
					const int
						mod = monthIndex(startDate) % recurringAmount,
						previousDateMonthIndex = monthIndex(previousDate),
						dateMonthIndex = monthIndex(date),
						divDifference
							= (dateMonthIndex-mod) / recurringAmount
							- (previousDateMonthIndex-mod) / recurringAmount;
					return divDifference >= 0 && (
						divDifference == 0 ? (
							dateMonthIndex == previousDateMonthIndex
							&& (dateMonthIndex-mod) % recurringAmount == 0
							&& startDate.day() > previousDate.day()
							&& startDate.day() <= date.day()
						) : (
							divDifference != 1
							|| (dateMonthIndex-mod) % recurringAmount != 0
							|| date.day() >= startDate.day()
						)
					);
					#undef monthIndex
				}
				case RecurringUnit::YEARS: {
					const int
						mod = startDate.year() % recurringAmount,
						divDifference
							= (date.year()-mod) / recurringAmount
							- (previousDate.year()-mod) / recurringAmount;
					return divDifference >= 0 && (
						divDifference == 0 ? (
							date.year() == previousDate.year()
							&& (date.year()-mod) % recurringAmount == 0
							&& (startDate.month() > previousDate.month() || (
								startDate.month() == previousDate.month() && startDate.day() > previousDate.day()
							))
							&& (startDate.month() < date.month() || (
								startDate.month() == date.month() && startDate.day() <= date.day()
							))
						) : (
							divDifference != 1
							|| (date.year()-mod) % recurringAmount != 0
							|| date.month() > startDate.month()
							|| (date.month() == startDate.month() && date.day() >= startDate.day())
						)
					);
				}
			}
			return difference >= 0 && difference % recurringAmount == 0;
		} else {
			return startDate.index() > previousDate.index() && startDate.index() <= date.index();
		}
	}
}