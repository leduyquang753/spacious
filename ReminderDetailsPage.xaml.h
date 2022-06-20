#pragma once

#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.ApplicationModel.Resources.h>

#include "ReminderDetailsPage.g.h"
#include "App.xaml.h"

namespace winrt::Spacious::implementation {
	struct ReminderDetailsPage: ReminderDetailsPageT<ReminderDetailsPage> {
		private:
			winrt::Windows::ApplicationModel::Resources::ResourceLoader &resourceLoader
				= App::instance->resourceLoader;
			bool
				validationDisabled = true,
				beginningPickingDate = false,
				updating = false;

			void updateName();
			void updateType();
			void validate();
			void setError(const hstring &key);
			void clearError();
		public:
			ReminderDetailsPage(const bool creating);
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
	};
}

namespace winrt::Spacious::factory_implementation {
	struct ReminderDetailsPage: ReminderDetailsPageT<ReminderDetailsPage, implementation::ReminderDetailsPage> {};
}