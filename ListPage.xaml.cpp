﻿#include "pch.h"

#include <winrt/Microsoft.UI.Xaml.Media.Animation.h>

#include "Reminder.h"
#include "ReminderStore.h"

#include "ListPage.xaml.h"
#if __has_include("ListPage.g.cpp")
	#include "ListPage.g.cpp"
#endif

#include "ReminderDetailsPage.xaml.h"

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
			list.Append(winrt::box_value(ReminderDisplayEntry{reminder.name.c_str()}));
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
		list.Append(winrt::box_value(ReminderDisplayEntry{reminder.name.c_str()}));
		updating = false;
		store.save();
	}

	void ListPage::setReminder(const int index, const ::Spacious::Reminder &reminder) {
		const int id = store.reminders[index].id;
		store.reminders[index] = reminder;
		store.reminders[index].id = id;
		updating = true;
		list.SetAt(index, winrt::box_value(ReminderDisplayEntry{reminder.name.c_str()}));
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
		DetailsPane().Content(winrt::Spacious::ReminderDetailsPage(*this, index));
		detailsPage = winrt::get_self<ReminderDetailsPage>(
			DetailsPane().Content().as<winrt::Spacious::ReminderDetailsPage>()
		);
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
		if (index == editingIndex) return;
		if (editingIndex != -2 && detailsPage->hasUnsavedChanges()) {
			showUnsavedDialog(index);
		} else {
			editReminder(index);
		}
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
		switch (co_await dialog.ShowAsync()) {
			case ContentDialogResult::Primary:
				detailsPage->save();
				editReminder(index);
				break;
			case ContentDialogResult::Secondary:
				editReminder(index);
				break;
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
		if (updating) return;
		const int index = ReminderList().SelectedIndex();
		tryEditReminder(index);
	}
}