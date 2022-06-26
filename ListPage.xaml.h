#pragma once

#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>
#include <winrt/Windows.Foundation.h>

#include "Reminder.h"
#include "ReminderStore.h"

#include "App.xaml.h"

#include "ListPage.g.h"

namespace winrt::Spacious::implementation {
	struct ReminderDetailsPage;
	
	struct ListPage: ListPageT<ListPage> {
		private:
			winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceLoader &resourceLoader
				= App::instance->resourceLoader;
			::Spacious::ReminderStore store;
			winrt::Windows::Foundation::Collections::IVector<IInspectable> list;
			ReminderDetailsPage *detailsPage = nullptr;
			int editingIndex = -2;
			bool updating = false;
			winrt::Windows::Foundation::IAsyncAction showUnsavedDialog(const int index);
		public:
			ListPage();
			winrt::Windows::Foundation::Collections::IVector<IInspectable> List();
			const ::Spacious::Reminder& getReminder(const int index);
			void addReminder(const ::Spacious::Reminder &reminder);
			void setReminder(const int index, const ::Spacious::Reminder &reminder);
			void deleteReminder(const int index);
			void editReminder(const int index);
			void tryEditReminder(const int index);
			void editReminderByID(const int id);
			void closeReminderDetails();
			void onCreateCommand(
				const winrt::Windows::Foundation::IInspectable &source,
				const winrt::Microsoft::UI::Xaml::RoutedEventArgs &args
			);
			void onSelectReminder(
				const winrt::Windows::Foundation::IInspectable &source,
				const winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs &args
			);
			void onReorderReminder(
				const winrt::Microsoft::UI::Xaml::Controls::ListViewBase &source,
				const winrt::Microsoft::UI::Xaml::Controls::DragItemsCompletedEventArgs &args
			);
	};
}

namespace winrt::Spacious::factory_implementation {
	struct ListPage: ListPageT<ListPage, implementation::ListPage> {};
}