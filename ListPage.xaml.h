#pragma once

#include <vector>

#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>

#include "Reminder.h"

#include "App.xaml.h"
#include "ReminderDetailsPage.xaml.h"

#include "ListPage.g.h"

namespace winrt::Spacious::implementation {
	struct ListPage: ListPageT<ListPage> {
		private:
			winrt::Windows::ApplicationModel::Resources::ResourceLoader &resourceLoader
				= App::instance->resourceLoader;
			std::vector<::Spacious::Reminder> reminders;
			winrt::Windows::Foundation::Collections::IVector<IInspectable> list;
			int nextID = 1;
			ReminderDetailsPage *detailsPage = nullptr;
			int editingIndex = -2;
			bool updating = false;
			void tryEditReminder(const int index);
			winrt::Windows::Foundation::IAsyncAction showUnsavedDialog(const int index);
		public:
			ListPage();
			winrt::Windows::Foundation::Collections::IVector<IInspectable> List();
			const ::Spacious::Reminder& getReminder(const int index);
			void addReminder(const ::Spacious::Reminder &reminder);
			void setReminder(const int index, const ::Spacious::Reminder &reminder);
			void deleteReminder(const int index);
			void editReminder(const int index);
			void closeReminderDetails();
			void onCreateCommand(
				const winrt::Windows::Foundation::IInspectable &source,
				const winrt::Microsoft::UI::Xaml::RoutedEventArgs &args
			);
			void onSelectReminder(
				const winrt::Windows::Foundation::IInspectable &source,
				const winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs &args
			);
	};
}

namespace winrt::Spacious::factory_implementation {
	struct ListPage: ListPageT<ListPage, implementation::ListPage> {};
}