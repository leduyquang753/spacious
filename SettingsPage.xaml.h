#pragma once

#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>
#include <winrt/Windows.Foundation.h>

#include "App.xaml.h"

#include "SettingsPage.g.h"

namespace winrt::Spacious::implementation {
	struct SettingsPage: SettingsPageT<SettingsPage> {
		private:
			winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceLoader &resourceLoader
				= App::instance->resourceLoader;
			struct {
				bool enableNotifications;
				int notificationHour, notificationMinute;
			} originalSettings;
			bool updating = false;
			void loadNotificationSettings();
			void save();
		public:
			SettingsPage();
			void onNotificationTimeElementChange(
				const winrt::Microsoft::UI::Xaml::Controls::NumberBox &source,
				const winrt::Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs &args
			);
			void onCreateSchedulerTask(
				const winrt::Windows::Foundation::IInspectable &source,
				const winrt::Microsoft::UI::Xaml::RoutedEventArgs &args
			);
			void onRevertChanges(
				const winrt::Windows::Foundation::IInspectable &source,
				const winrt::Microsoft::UI::Xaml::RoutedEventArgs &args
			);
			void onSave(
				const winrt::Windows::Foundation::IInspectable &source,
				const winrt::Microsoft::UI::Xaml::RoutedEventArgs &args
			);
			void onCreateStartShortcut(
				const winrt::Windows::Foundation::IInspectable &source,
				const winrt::Microsoft::UI::Xaml::RoutedEventArgs &args
			);
			winrt::Windows::Foundation::IAsyncAction onNavigateOut(const int id);
	};
}

namespace winrt::Spacious::factory_implementation {
	struct SettingsPage: SettingsPageT<SettingsPage, implementation::SettingsPage> {};
}