#include <array>
#include <stdexcept>
#include <string>

#include "Date.h"

namespace Spacious {
	using namespace std::string_literals;

	Date::InvalidDateException::InvalidDateException(const int day, const int month, const int year):
		message("day="s + std::to_string(day) + "; month="s + std::to_string(month) + "; year="s + std::to_string(year))
	{}

	Date::InvalidDateException::InvalidDateException(const int index):
		message("index="s + std::to_string(index))
	{}

	const char* Date::InvalidDateException::what() const noexcept {
		return message.c_str();
	}

	const std::array<int, 13>
		Date::monthDays = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
		Date::monthDaysLeap = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	const std::array<int, 14>
		Date::monthPos = {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
		Date::monthPosLeap = {0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};
	int
		Date::minIndex = Date(1, 1, 1900).index(),
		Date::maxIndex = Date(31, 12, 2100).index();

	bool Date::isLeapYear(const int year) {
		return year%4==0 && (year%100!=0 || year%400==0);
	}

	bool Date::isValidDate(const int day, const int month, const int year) {
		return
			year >= 1900 && year <= 2100
			&& month >= 1 && month <= 12
			&& day >= 1 && day <= (isLeapYear(year) ? monthDaysLeap : monthDays)[month];
	}

	Date::Date(const int day, const int month, const int year): m_day(day), m_month(month), m_year(year) {
		if (!isValidDate(day, month, year)) throw InvalidDateException(day, month, year);
		m_index
			= (year - 1) * 365
			+ year / 4 - year / 100 + year / 400
			- (isLeapYear(year) && month < 3 ? 2 : 1) + monthPos[month] + day;
	}

	Date::Date(const int index): m_index(index) {
		if (index < minIndex || index > maxIndex)
			throw InvalidDateException(index);
		int cycles, dayOfYear = 0;
		m_year = (cycles = index/146097) * 400; // Gregorian calendar repeats every 146 097 days, or 400 years.
		if (index % 146097 == 146096) {
			// Handle the last day of the cycle, which is the 366th day of the 400th year.
			m_year += 400;
			dayOfYear = 365;
		} else {
			int temp = index - cycles*146097;
			// In each repeat cycle, it repeats every 100 years, or 36 524 days; the only irregular year is the
			// 400th year which is a leap year.
			m_year += (cycles = temp / 36524) * 100;
			// In that sub-cycle, it also repeats every 4 years or 1461 days, except the 100th which is not a leap year.
			m_year += (cycles = (temp -= cycles*36524) / 1461) * 4;
			// In that sub-sub-cycle, it also repeats every year, or 365 days, except the 4th which is a leap year.
			m_year += (cycles = (temp -= cycles*1461) / 365);
			// Handle the last day of the 4-year cycle.
			m_year += (cycles == 4 ? 0 : 1);
			dayOfYear = cycles == 4 ? 365 : temp - cycles*365;
		}
		auto &table = isLeapYear(m_year) ? monthPosLeap : monthPos;
		for (m_month = 1; m_month < 12; ++m_month) if (dayOfYear < table[m_month+1]) break;
		m_day = dayOfYear - table[m_month] + 1;
	}

	int Date::day() {
		return m_day;
	}

	int Date::month() {
		return m_month;
	}

	int Date::year() {
		return m_year;
	}

	int Date::index() {
		return m_index;
	}
}