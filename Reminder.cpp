#include "Date.h"

#include "Reminder.h"

namespace Spacious {
	#define monthIndex(d) (d.year()*12 + d.month() - 1)

	Date Reminder::getNextNotification(
		bool isRecurring, const Date &startDate, int recurringAmount, RecurringUnit recurringUnit,
		const Date &today
	) {
		if (!isRecurring || today.index() < startDate.index()) return startDate;
		switch (recurringUnit) {
			case RecurringUnit::DAYS: {
				return {
					startDate.index()
					+ (today.index()-startDate.index()+recurringAmount) / recurringAmount * recurringAmount
				};
			}
			case RecurringUnit::MONTHS: {
				const int
					startMonthIndex = monthIndex(startDate),
					currentMonthIndex = monthIndex(today);
				int nextMonthIndex
					= startMonthIndex
					+ (currentMonthIndex-startMonthIndex) / recurringAmount * recurringAmount;
				if (
					currentMonthIndex > nextMonthIndex
					|| (currentMonthIndex == nextMonthIndex && today.day() >= startDate.day())
				) nextMonthIndex += recurringAmount;
				return Date::clamped(startDate.day(), nextMonthIndex % 12 + 1, nextMonthIndex / 12);
			}
			case RecurringUnit::YEARS: {
				int nextYear
					= startDate.year()
					+ (today.year()-startDate.year()) / recurringAmount * recurringAmount;
				if (
					today.year() > nextYear
					|| (today.year() == nextYear && (
						today.month() > startDate.month()
						|| (today.month() == startDate.month() && today.day() >= startDate.day())
					))
				) nextYear += recurringAmount;
				return Date::clamped(startDate.day(), startDate.month(), nextYear);
			}
			default: {
				return startDate;
			}
		}
	}
	
	bool Reminder::hasNotification(const Date &previousDate, const Date &date) const {
		if (date.index() < startDate.index()) return false;
		if (isRecurring) {
			switch (recurringUnit) {
				case RecurringUnit::DAYS: {
					const int mod = startDate.index() % recurringAmount;
					return (previousDate.index()-mod) / recurringAmount < (date.index()-mod) / recurringAmount;
				}
				case RecurringUnit::MONTHS: {
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
				default: return false;
			}
		} else {
			return startDate.index() > previousDate.index();
		}
	}

	#undef monthIndex
}