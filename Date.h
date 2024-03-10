#pragma once

#include <array>
#include <stdexcept>
#include <string>

namespace Spacious {
	class Date {
		private:
			static const std::array<int, 13> monthDays, monthDaysLeap;
			static const std::array<int, 14> monthPos, monthPosLeap;
			static int minIndex, maxIndex;
			static bool isLeapYear(const int year);

			int m_day, m_month, m_year, m_index;
		public:
			class InvalidDateException: public std::exception {
				private:
					std::string message;
				public:
					InvalidDateException(const int day, const int month, const int year);
					InvalidDateException(const int index);
					const char* what() const noexcept override;
			};

			static bool isValidDate(const int day, const int month, const int year);
			static Date today();
			static Date clamped(const int day, const int month, const int year);

			Date(const int day, const int month, const int year);
			Date(const int index);
			int day() const;
			int month() const;
			int year() const;
			int index() const;
	};
}