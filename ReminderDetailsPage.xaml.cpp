#include "pch.h"

#include <ctime>
#include <string_view>

#include "Date.h"
#include "Reminder.h"

#include "ReminderDetailsPage.xaml.h"
#if __has_include("ReminderDetailsPage.g.cpp")
	#include "ReminderDetailsPage.g.cpp"
#endif

using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Documents;
using namespace winrt::Microsoft::UI::Xaml::Markup;
using namespace winrt::Windows::Foundation;

namespace winrt::Spacious::implementation {
	ReminderDetailsPage::ReminderDetailsPage(const winrt::Spacious::ListPage &parentProjection):
		parent(*winrt::get_self<ListPage>(parentProjection))
	{
		const int index = parent.getEditingIndex();
		
		InitializeComponent();
		CreatingButtons().Visibility(index == -1 ? Visibility::Visible : Visibility::Collapsed);
		EditingButtons().Visibility(index == -1 ? Visibility::Collapsed : Visibility::Visible);
		
		if (index == -1) {
			const auto timeT = std::time(nullptr);
			const auto &cTime = *std::localtime(&timeT);
			StartDay().Value(currentDay = cTime.tm_mday);
			StartMonth().Value(currentMonth = cTime.tm_mon + 1);
			StartYear().Value(currentYear = cTime.tm_year + 1900);
		} else {
			loadReminderInfo();
		}
		
		updateName();
		updateType();
		
		validationDisabled = false;
		validate();
	}

	void ReminderDetailsPage::loadReminderInfo() {
		const auto &reminder = parent.getReminder(parent.getEditingIndex());
		Name().Text(reminder.name.c_str());
		Type().IsOn(reminder.isRecurring);
		StartDay().Value(reminder.startDate.day());
		StartMonth().Value(reminder.startDate.month());
		StartYear().Value(reminder.startDate.year());
		RecurringAmount().Value(reminder.isRecurring ? reminder.recurringAmount : 1);
		RecurringUnit().SelectedIndex(reminder.isRecurring ? static_cast<int>(reminder.recurringUnit) : 0);
		NotificationText().Text(reminder.notificationText.c_str());
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

	bool ReminderDetailsPage::hasUnsavedChanges() {
		const int editingIndex = parent.getEditingIndex();
		if (editingIndex == -1) {
			return
				!Name().Text().empty()
				|| Type().IsOn()
				|| static_cast<int>(StartDay().Value()) != currentDay
				|| static_cast<int>(StartMonth().Value()) != currentMonth
				|| static_cast<int>(StartYear().Value()) != currentYear
				|| static_cast<int>(RecurringAmount().Value()) != 1
				|| RecurringUnit().SelectedIndex() != 0
				|| !NotificationText().Text().empty();
		} else {
			const auto &reminder = parent.getReminder(editingIndex);
			return
				Name().Text() != reminder.name.c_str()
				|| Type().IsOn() != reminder.isRecurring
				|| static_cast<int>(StartDay().Value()) != reminder.startDate.day()
				|| static_cast<int>(StartMonth().Value()) != reminder.startDate.month()
				|| static_cast<int>(StartYear().Value()) != reminder.startDate.year()
				|| (Type().IsOn() && (
					static_cast<int>(RecurringAmount().Value()) != reminder.recurringAmount
					|| RecurringUnit().SelectedIndex() != static_cast<int>(reminder.recurringUnit)
				))
				|| NotificationText().Text() != reminder.notificationText.c_str();
		}
	}

	void ReminderDetailsPage::save() {
		::Spacious::Reminder reminder = {
			0,
			std::wstring(static_cast<std::wstring_view>(Name().Text())),
			Type().IsOn(),
			{
				static_cast<int>(StartDay().Value()),
				static_cast<int>(StartMonth().Value()),
				static_cast<int>(StartYear().Value())
			},
			static_cast<int>(RecurringAmount().Value()),
			static_cast<::Spacious::Reminder::RecurringUnit>(
				static_cast<int>(RecurringUnit().SelectedIndex())
			),
			std::wstring(static_cast<std::wstring_view>(NotificationText().Text()))
		};
		const int editingIndex = parent.getEditingIndex();
		if (editingIndex == -1) {
			parent.addReminder(reminder);
			parent.closeReminderDetails();
		} else {
			parent.setReminder(editingIndex, reminder);
		}
	}
	
	IAsyncAction ReminderDetailsPage::del() {
		ContentDialog dialog;
		dialog.XamlRoot(XamlRoot());
		dialog.Style(
			Application::Current().Resources()
				.Lookup(winrt::box_value(L"DefaultContentDialogStyle"))
				.as<winrt::Microsoft::UI::Xaml::Style>()
		);
		TextBlock content;
		content.Text(resourceLoader.GetString(L"ReminderDetailsPage_Dialog_SureToDelete"));
		dialog.Content(content);
		dialog.PrimaryButtonText(resourceLoader.GetString(L"ReminderDetailsPage_Dialog_Delete"));
		dialog.IsSecondaryButtonEnabled(false);
		dialog.CloseButtonText(resourceLoader.GetString(L"ReminderDetailsPage_Dialog_Cancel"));
		dialog.DefaultButton(ContentDialogButton::Primary);
		if (co_await dialog.ShowAsync() == ContentDialogResult::Primary) {
			parent.deleteReminder(parent.getEditingIndex());
			parent.closeReminderDetails();
		}
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
	
	void ReminderDetailsPage::onRevertChanges(const IInspectable &source, const RoutedEventArgs &args) {
		loadReminderInfo();
	}
	
	void ReminderDetailsPage::onSave(const IInspectable &source, const RoutedEventArgs &args) {
		save();
	}
	
	void ReminderDetailsPage::onDelete(const IInspectable &source, const RoutedEventArgs &args) {
		del();
	}
}