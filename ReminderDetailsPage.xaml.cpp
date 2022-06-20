#include "pch.h"

#include <chrono>
#include <ctime>

#include "Date.h"

#include "ReminderDetailsPage.xaml.h"
#if __has_include("ReminderDetailsPage.g.cpp")
	#include "ReminderDetailsPage.g.cpp"
#endif

using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Windows::Foundation;

namespace winrt::Spacious::implementation {
	ReminderDetailsPage::ReminderDetailsPage(const bool creating) {
		InitializeComponent();
		CreatingButtons().Visibility(creating ? Visibility::Visible : Visibility::Collapsed);
		EditingButtons().Visibility(creating ? Visibility::Collapsed : Visibility::Visible);
		updateName();
		updateType();
		
		{
			const auto timeT = std::time(nullptr);
			const auto &cTime = *std::localtime(&timeT);
			StartDay().Value(cTime.tm_mday);
			StartMonth().Value(cTime.tm_mon + 1);
			StartYear().Value(cTime.tm_year + 1900);
		}
		
		validationDisabled = false;
		validate();
	}

	void ReminderDetailsPage::updateName() {
		NotificationText().PlaceholderText(
			Name().Text().empty()
				? resourceLoader.GetString(L"ReminderDetailsPage_NotificationText_EmptyNamePlaceholder")
				: Name().Text()
		);
	}

	void ReminderDetailsPage::updateType() {
		const bool isRecurring = Type().IsOn();
		StartDateLabel().Text(resourceLoader.GetString(
			isRecurring
				? L"ReminderDetailsPage_StartDateLabel_Recurring"
				: L"ReminderDetailsPage_StartDateLabel_NonRecurring"
		));
		RecurringPeriodPanel().Visibility(isRecurring ? Visibility::Visible : Visibility::Collapsed);
	}

	void ReminderDetailsPage::validate() {
		if (validationDisabled) return;
		if (Name().Text().empty()) {
			setError(L"ReminderDetailsPage_Error_NoName");
			return;
		}
		if (!::Spacious::Date::isValidDate(
			static_cast<int>(StartDay().Value()),
			static_cast<int>(StartMonth().Value()),
			static_cast<int>(StartYear().Value())
		)) {
			setError(L"ReminderDetailsPage_Error_InvalidDate");
			return;
		}
		clearError();
	}

	void ReminderDetailsPage::setError(const hstring &key) {
		Error().Text(resourceLoader.GetString(key));
		CreateButton().IsEnabled(false);
		SaveButton().IsEnabled(false);
	}

	void ReminderDetailsPage::clearError() {
		Error().Text(L"");
		CreateButton().IsEnabled(true);
		SaveButton().IsEnabled(true);
	}

	void ReminderDetailsPage::onNameChange(const IInspectable &source, const RoutedEventArgs &args) {
		updateName();
		validate();
	}

	void ReminderDetailsPage::onTypeChange(const IInspectable &source, const RoutedEventArgs &args) {
		updateType();
	}

	void ReminderDetailsPage::onStartDateElementChange(
		const NumberBox &source, const NumberBoxValueChangedEventArgs &args
	) {
		if (updating) return;
		updating = true;
		source.Value(static_cast<int>(source.Value()));
		validate();
		updating = false;
	}
	
	void ReminderDetailsPage::onBeginPickDate(const IInspectable &source, const RoutedEventArgs &args) {
		beginningPickingDate = true;
		auto selectedDates = DatePicker().SelectedDates();
		selectedDates.Clear();
		const int
			day = static_cast<int>(StartDay().Value()),
			month = static_cast<int>(StartMonth().Value()),
			year = static_cast<int>(StartYear().Value());
		if (::Spacious::Date::isValidDate(day, month, year)) {
			std::tm cDate{};
			cDate.tm_mday = day;
			cDate.tm_mon = month - 1;
			cDate.tm_year = year - 1900;
			selectedDates.Append(clock::from_time_t(std::mktime(&cDate)));
		}
		beginningPickingDate = false;
	}

	void ReminderDetailsPage::onPickDate(
		const winrt::Microsoft::UI::Xaml::Controls::CalendarView &source,
		const winrt::Microsoft::UI::Xaml::Controls::CalendarViewSelectedDatesChangedEventArgs &args
	) {
		if (beginningPickingDate) return;
		validationDisabled = true;
		const auto timeT = winrt::clock::to_time_t(DatePicker().SelectedDates().GetAt(0));
		const auto &cTime = *std::localtime(&timeT);
		StartDay().Value(cTime.tm_mday);
		StartMonth().Value(cTime.tm_mon + 1);
		StartYear().Value(cTime.tm_year + 1900);
		validationDisabled = false;
		validate();
	}
	
	void ReminderDetailsPage::onRecurringAmountChange(
		const winrt::Microsoft::UI::Xaml::Controls::NumberBox &source,
		const winrt::Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs &args
	) {
		if (updating) return;
		updating = true;
		source.Value(static_cast<int>(source.Value()));
		updating = false;
	}
}