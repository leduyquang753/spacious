#pragma once

#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>
#include <winrt/Windows.Foundation.h>

#include "App.xaml.h"
#include "ListPage.xaml.h"

#include "ReminderDetailsPage.g.h"

namespace winrt::Spacious::implementation {
	struct ReminderDetailsPage: ReminderDetailsPageT<ReminderDetailsPage> {
		private:
			ListPage &parent;
			winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceLoader &resourceLoader
				= App::instance->resourceLoader;
			bool
				validationDisabled = true,
				beginningPickingDate = false,
				updating = false;
			int currentDay = 1, currentMonth = 1, currentYear = 2022;

			void loadReminderInfo();
			void updateName();
			void updateType();
			void validate();
			void setError(const hstring &key);
			void clearError();
			winrt::Windows::Foundation::IAsyncAction del();
		public:
			ReminderDetailsPage(const winrt::Spacious::ListPage &parent);
			bool hasUnsavedChanges();
			void save();
			void onNameChange(
				const winrt::Windows::Foundation::IInspectable &source,
				const winrt::Microsoft::UI::Xaml::RoutedEventArgs &args
			);
			void onTypeChange(
				const winrt::Windows::Foundation::IInspectable &source,
				const winrt::Microsoft::UI::Xaml::RoutedEventArgs &args
			);
			void onStartDateElementChange(
				const winrt::Microsoft::UI::Xaml::Controls::NumberBox &source,
				const winrt::Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs &args
			);
			void onBeginPickDate(
				const winrt::Windows::Foundation::IInspectable &source,
				const winrt::Microsoft::UI::Xaml::RoutedEventArgs &args
			);
			void onPickDate(
				const winrt::Microsoft::UI::Xaml::Controls::CalendarView &source,
				const winrt::Microsoft::UI::Xaml::Controls::CalendarViewSelectedDatesChangedEventArgs &args
			);
			void onRecurringAmountChange(
				const winrt::Microsoft::UI::Xaml::Controls::NumberBox &source,
				const winrt::Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs &args
			);
			void onRevertChanges(
				const winrt::Windows::Foundation::IInspectable &source,
				const winrt::Microsoft::UI::Xaml::RoutedEventArgs &args
			);
			void onSave(
				const winrt::Windows::Foundation::IInspectable &source,
				const winrt::Microsoft::UI::Xaml::RoutedEventArgs &args
			);
			void onDelete(
				const winrt::Windows::Foundation::IInspectable &source,
				const winrt::Microsoft::UI::Xaml::RoutedEventArgs &args
			);
	};
}

namespace winrt::Spacious::factory_implementation {
	struct ReminderDetailsPage: ReminderDetailsPageT<ReminderDetailsPage, implementation::ReminderDetailsPage> {};
}