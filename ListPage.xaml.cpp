#include "pch.h"

#include <winrt/Microsoft.UI.Xaml.Media.Animation.h>

#include "Reminder.h"
#include "ReminderStore.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include "ReminderDetailsPage.xaml.h"

#include "ListPage.xaml.h"
#if __has_include("ListPage.g.cpp")
	#include "ListPage.g.cpp"
#endif

using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Documents;
using namespace winrt::Microsoft::UI::Xaml::Markup;
using namespace winrt::Microsoft::UI::Xaml::Media::Animation;
using namespace winrt::Windows::Foundation::Collections;

namespace winrt::Spacious::implementation {
	ListPage::ListPage(): list(winrt::single_threaded_observable_vector<IInspectable>()) {
		InitializeComponent();

		store.load();
		for (const auto &reminder : store.reminders)
			list.Append(winrt::box_value(ReminderDisplayEntry{reminder.name.c_str(), reminder.id}));
		ReminderList().ItemsSource(List());

		DetailsPane().Navigate(winrt::xaml_typename<NoReminderSelectedPage>());
	}
	
	IVector<IInspectable> ListPage::List() {
		return list;
	}

	const ::Spacious::Reminder& ListPage::getReminder(const int index) {
		return store.reminders[index];
	}

	void ListPage::addReminder(const ::Spacious::Reminder &reminder) {
		store.reminders.push_back(reminder);
		store.reminders.back().id = store.nextID++;
		updating = true;
		list.Append(winrt::box_value(ReminderDisplayEntry{reminder.name.c_str(), store.reminders.back().id}));
		updating = false;
		store.save();
	}

	void ListPage::setReminder(const int index, const ::Spacious::Reminder &reminder) {
		const int id = store.reminders[index].id;
		store.reminders[index] = reminder;
		store.reminders[index].id = id;
		updating = true;
		list.SetAt(index, winrt::box_value(ReminderDisplayEntry{reminder.name.c_str(), store.reminders[index].id}));
		ReminderList().SelectedIndex(index);
		updating = false;
		store.save();
	}

	void ListPage::deleteReminder(const int index) {
		store.reminders.erase(store.reminders.begin() + index);
		updating = true;
		list.RemoveAt(index);
		updating = false;
		store.save();
	}

	void ListPage::editReminder(const int index) {
		editingIndex = index;
		DetailsPane().Content(winrt::Spacious::ReminderDetailsPage(*this));
		detailsPage = DetailsPane().Content().as<winrt::Spacious::implementation::ReminderDetailsPage>().get();
		updating = true;
		ReminderList().SelectedIndex(index);
		updating = false;
	}

	void ListPage::closeReminderDetails() {
		DetailsPane().Navigate(
			winrt::xaml_typename<NoReminderSelectedPage>(),
			nullptr,
			SuppressNavigationTransitionInfo()
		);
		updating = true;
		ReminderList().SelectedIndex(-1);
		updating = false;
		editingIndex = -2;
	}

	void ListPage::tryEditReminder(const int index) {
		if (index != -2 && index == editingIndex) return;
		if (editingIndex != -2 && detailsPage->hasUnsavedChanges()) {
			showUnsavedDialog(index);
		} else {
			if (index == -2) App::instance->window.as<MainWindow>()->close();
			else editReminder(index);
		}
	}

	void ListPage::editReminderByID(const int id) {
		if (id == -1) {
			tryEditReminder(-1);
			return;
		}
		int index = 0;
		for (const auto &reminder : store.reminders) {
			if (reminder.id == id) {
				tryEditReminder(index);
				return;
			}
			++index;
		}
	}

	int ListPage::getEditingIndex() {
		return editingIndex;
	}

	winrt::Windows::Foundation::IAsyncAction ListPage::showUnsavedDialog(const int index) {
		updating = true;
		ReminderList().SelectedIndex(editingIndex);
		updating = false;
		ContentDialog dialog;
		dialog.XamlRoot(XamlRoot());
		dialog.Style(
			Application::Current().Resources()
				.Lookup(winrt::box_value(L"DefaultContentDialogStyle"))
				.as<winrt::Microsoft::UI::Xaml::Style>()
		);
		RichTextBlock content;
		content.Blocks().Append(XamlReader::Load(resourceLoader.GetString(
			editingIndex == -1
				? L"ReminderDetailsPage_Dialog_UnsavedCreation"
				: L"ReminderDetailsPage_Dialog_UnsavedEdit"
		)).as<Paragraph>());
		content.IsTextSelectionEnabled(false);
		dialog.Content(content);
		dialog.PrimaryButtonText(resourceLoader.GetString(L"ReminderDetailsPage_Dialog_Save"));
		dialog.SecondaryButtonText(resourceLoader.GetString(L"ReminderDetailsPage_Dialog_Discard"));
		dialog.CloseButtonText(resourceLoader.GetString(L"ReminderDetailsPage_Dialog_Cancel"));
		dialog.DefaultButton(ContentDialogButton::Primary);
		auto response = co_await dialog.ShowAsync();
		if (response == ContentDialogResult::Primary) detailsPage->save();
		if (response != ContentDialogResult::None) {
			if (index == -2) App::instance->window.Close();
			else editReminder(index);
		}
	}
	
	void ListPage::onCreateCommand(
		const winrt::Windows::Foundation::IInspectable &source,
		const winrt::Microsoft::UI::Xaml::RoutedEventArgs &args
	) {
		tryEditReminder(-1);
	}
	
	void ListPage::onSelectReminder(
		const winrt::Windows::Foundation::IInspectable &source,
		const winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs &args
	) {
		if (updating || App::instance->activatingFromToast) return;
		const int index = ReminderList().SelectedIndex();
		tryEditReminder(index);
	}
	
	void ListPage::onReorderReminder(
		const winrt::Microsoft::UI::Xaml::Controls::ListViewBase &source,
		const winrt::Microsoft::UI::Xaml::Controls::DragItemsCompletedEventArgs &args
	) {
		const int id = args.Items().GetAt(0).as<ReminderDisplayEntry>().id;
		unsigned int newIndex, oldIndex, size = list.Size();
		for (newIndex = 0; newIndex != size; newIndex++)
			if (list.GetAt(newIndex).as<ReminderDisplayEntry>().id == id) break;
		size = static_cast<unsigned int>(store.reminders.size());
		for (oldIndex = 0; oldIndex != size; oldIndex++)
			if (store.reminders[oldIndex].id == id) break;
		if (newIndex == oldIndex) return;
		auto reminder = store.reminders[oldIndex];
		store.reminders.erase(store.reminders.begin() + oldIndex);
		store.reminders.insert(store.reminders.begin() + newIndex, reminder);
		store.save();
		if (editingIndex >= 0) {
			if (static_cast<unsigned int>(editingIndex) == oldIndex) {
				editingIndex = newIndex;
			} else {
				if (oldIndex < static_cast<unsigned int>(editingIndex)) --editingIndex;
				if (newIndex <= static_cast<unsigned int>(editingIndex)) ++editingIndex;
			}
		}
	}
}