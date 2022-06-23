#include "pch.h"

#include <cstdlib>
#include <string>

#include <combaseapi.h>
#include <comutil.h>
#include <oleauto.h>
#include <taskschd.h>
#include <winrt/Windows.Globalization.NumberFormatting.h>

#include "Version.h"

#include "ListPage.xaml.h"
#include "MainWindow.xaml.h"

#include "SettingsPage.xaml.h"
#if __has_include("SettingsPage.g.cpp")
	#include "SettingsPage.g.cpp"
#endif

using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Documents;
using namespace winrt::Microsoft::UI::Xaml::Markup;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Globalization::NumberFormatting;

namespace winrt::Spacious::implementation {
	#define check() winrt::check_hresult(r)
	
	SettingsPage::SettingsPage() {
		InitializeComponent();

		DecimalFormatter formatter;
		formatter.IntegerDigits(2);
		formatter.FractionDigits(0);
		NotificationMinute().NumberFormatter(formatter);

		Version().Text(resourceLoader.GetString(L"SettingsPage_Version") + ::Spacious::versionString);

		loadNotificationSettings();
	}

	void SettingsPage::loadNotificationSettings() {
		HRESULT r;
		ITaskService *taskService;
		r = CoCreateInstance(
			CLSID_TaskScheduler, nullptr, CLSCTX_INPROC_SERVER, IID_ITaskService,
			reinterpret_cast<void**>(&taskService)
		);
		check();
		r = taskService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
		check();
		ITaskFolder *taskFolder;
		r = taskService->GetFolder(_bstr_t(L"\\Spacious"), &taskFolder);
		if (SUCCEEDED(r)) {
			IRegisteredTask *task;
			r = taskFolder->GetTask(_bstr_t(L"Reminder notification"), &task);
			if (SUCCEEDED(r)) {
				NoScheduledTaskSection().Visibility(Visibility::Collapsed);
				SettingsSection().Visibility(Visibility::Visible);
				ButtonsSection().Visibility(Visibility::Visible);
				VARIANT_BOOL enabled;
				r = task->get_Enabled(&enabled);
				check();
				EnableNotifications().IsOn(enabled);
				originalSettings.enableNotifications = enabled;
				ITaskDefinition *taskDefinition;
				r = task->get_Definition(&taskDefinition);
				check();
				ITriggerCollection *triggers;
				r = taskDefinition->get_Triggers(&triggers);
				check();
				ITrigger *trigger;
				r = triggers->get_Item(1, &trigger);
				check();
				BSTR startTimeOLEString;
				r = trigger->get_StartBoundary(&startTimeOLEString);
				check();
				std::wstring startTimeString(startTimeOLEString, SysStringLen(startTimeOLEString));
				SysFreeString(startTimeOLEString);
				trigger->Release();
				triggers->Release();
				taskDefinition->Release();
				task->Release();
				updating = true;
				originalSettings.notificationHour = std::stoi(startTimeString.substr(11, 2));
				NotificationHour().Value(originalSettings.notificationHour);
				originalSettings.notificationMinute = std::stoi(startTimeString.substr(14, 2));
				NotificationMinute().Value(originalSettings.notificationMinute);
				updating = false;
				r = false;
			} else {
				r = true;
			}
			taskFolder->Release();
		} else {
			r = true;
		}
		if (r) {
			NoScheduledTaskSection().Visibility(Visibility::Visible);
			SettingsSection().Visibility(Visibility::Collapsed);
			ButtonsSection().Visibility(Visibility::Collapsed);
		}
		taskService->Release();
	}

	void SettingsPage::save() {
		HRESULT r;
		ITaskService *taskService;
		r = CoCreateInstance(
			CLSID_TaskScheduler, nullptr, CLSCTX_INPROC_SERVER, IID_ITaskService,
			reinterpret_cast<void**>(&taskService)
		);
		check();
		r = taskService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
		check();
		ITaskFolder *taskFolder;
		r = taskService->GetFolder(_bstr_t(L"\\Spacious"), &taskFolder);
		check();
		IRegisteredTask *task;
		r = taskFolder->GetTask(_bstr_t(L"Reminder notification"), &task);
		check();
		ITaskDefinition *taskDefinition;
		r = task->get_Definition(&taskDefinition);
		check();
		ITriggerCollection *triggers;
		r = taskDefinition->get_Triggers(&triggers);
		check();
		ITrigger *trigger;
		r = triggers->get_Item(1, &trigger);
		check();
		IDailyTrigger *dailyTrigger;
		r = trigger->QueryInterface(IID_IDailyTrigger, reinterpret_cast<void**>(&dailyTrigger));
		check();
		const int
			notificationHour = static_cast<int>(NotificationHour().Value()),
			notificationMinute = static_cast<int>(NotificationMinute().Value());
		std::wstring startTimeString = L"2001-01-01T";
		if (notificationHour < 10) startTimeString += L'0';
		startTimeString += std::to_wstring(notificationHour);
		startTimeString += L':';
		if (notificationMinute < 10) startTimeString += L'0';
		startTimeString += std::to_wstring(notificationMinute);
		startTimeString += L":00";
		r = dailyTrigger->put_StartBoundary(_bstr_t(startTimeString.c_str()));
		check();
		IRegisteredTask *newTask;
		r = taskFolder->RegisterTaskDefinition(
			_bstr_t(L"Reminder notification"),
			taskDefinition,
			TASK_UPDATE,
			_variant_t(), _variant_t(),
			TASK_LOGON_INTERACTIVE_TOKEN,
			_variant_t(L""),
			&newTask
		);
		check();
		r = newTask->put_Enabled(_variant_t(EnableNotifications().IsOn()));
		check();
		newTask->Release();
		dailyTrigger->Release();
		trigger->Release();
		triggers->Release();
		taskDefinition->Release();
		task->Release();
		taskFolder->Release();
		taskService->Release();

		originalSettings.enableNotifications = EnableNotifications().IsOn();
		originalSettings.notificationHour = notificationHour;
		originalSettings.notificationMinute = notificationMinute;
	}
	
	void SettingsPage::onNotificationTimeElementChange(
		const NumberBox &source, const NumberBoxValueChangedEventArgs &args
	) {
		if (updating) return;
		updating = true;
		source.Value(static_cast<int>(source.Value()));
		updating = false;
	}
	
	void SettingsPage::onCreateSchedulerTask(const IInspectable &source, const RoutedEventArgs &args) {
		HRESULT r;
		ITaskService *taskService;
		r = CoCreateInstance(
			CLSID_TaskScheduler, nullptr, CLSCTX_INPROC_SERVER, IID_ITaskService,
			reinterpret_cast<void**>(&taskService)
		);
		check();
		r = taskService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
		check();
		ITaskFolder *taskFolder;
		r = taskService->GetFolder(_bstr_t(L"\\Spacious"), &taskFolder);
		if (FAILED(r)) {
			ITaskFolder *rootFolder;
			r = taskService->GetFolder(_bstr_t(L"\\"), &rootFolder);
			check();
			r = rootFolder->CreateFolder(_bstr_t(L"Spacious"), _variant_t(L""), &taskFolder);
			check();
			rootFolder->Release();
		}
		ITaskDefinition *taskDefinition;
		r = taskService->NewTask(0, &taskDefinition);
		check();
		IRegistrationInfo *registrationInfo;
		r = taskDefinition->get_RegistrationInfo(&registrationInfo);
		check();
		r = registrationInfo->put_Author(_bstr_t(L"Spacious"));
		check();
		ITriggerCollection *triggers;
		r = taskDefinition->get_Triggers(&triggers);
		check();
		ITrigger *trigger;
		r = triggers->Create(TASK_TRIGGER_DAILY, &trigger);
		check();
		IDailyTrigger *dailyTrigger;
		r = trigger->QueryInterface(IID_IDailyTrigger, reinterpret_cast<void**>(&dailyTrigger));
		check();
		r = dailyTrigger->put_Id(_bstr_t(L"Main"));
		check();
		r = dailyTrigger->put_StartBoundary(_bstr_t(L"2001-01-01T08:00:00"));
		check();
		r = dailyTrigger->put_DaysInterval(static_cast<short>(1));
		check();
		IActionCollection *actions;
		r = taskDefinition->get_Actions(&actions);
		check();
		IAction *action;
		r = actions->Create(TASK_ACTION_EXEC, &action);
		check();
		IExecAction *execAction;
		r = action->QueryInterface(IID_IExecAction, reinterpret_cast<void**>(&execAction));
		check();
		wchar_t *programPath;
		_get_wpgmptr(&programPath);
		r = execAction->put_Path(_bstr_t(programPath));
		check();
		r = execAction->put_Arguments(_bstr_t(L"-SendNotifications"));
		check();
		ITaskSettings *settings;
		r = taskDefinition->get_Settings(&settings);
		check();
		r = settings->put_DisallowStartIfOnBatteries(_variant_t(false));
		check();
		r = settings->put_StartWhenAvailable(_variant_t(true));
		check();
		r = settings->put_ExecutionTimeLimit(_bstr_t(L"PT0S"));
		check();
		IRegisteredTask *task;
		r = taskFolder->RegisterTaskDefinition(
			_bstr_t(L"Reminder notification"),
			taskDefinition,
			TASK_CREATE_OR_UPDATE,
			_variant_t(), _variant_t(),
			TASK_LOGON_INTERACTIVE_TOKEN,
			_variant_t(L""),
			&task
		);
		check();
		task->Release();
		settings->Release();
		execAction->Release();
		action->Release();
		actions->Release();
		dailyTrigger->Release();
		trigger->Release();
		triggers->Release();
		registrationInfo->Release();
		taskDefinition->Release();
		taskFolder->Release();
		taskService->Release();
		
		loadNotificationSettings();
	}
	
	void SettingsPage::onRevertChanges(const IInspectable &source, const RoutedEventArgs &args) {
		updating = true;
		EnableNotifications().IsOn(originalSettings.enableNotifications);
		NotificationHour().Value(originalSettings.notificationHour);
		NotificationMinute().Value(originalSettings.notificationMinute);
		updating = false;
	}
	
	void SettingsPage::onSave(const IInspectable &source, const RoutedEventArgs &args) {
		save();
	}	

	winrt::Windows::Foundation::IAsyncAction SettingsPage::onNavigateOut(const int id) {
		auto window = App::instance->window.as<MainWindow>();
		auto navigationView = window->MainNavigationView();
		if (SettingsSection().Visibility() == Visibility::Visible && (
			EnableNotifications().IsOn() != originalSettings.enableNotifications
			|| static_cast<int>(NotificationHour().Value()) != originalSettings.notificationHour
			|| static_cast<int>(NotificationMinute().Value()) != originalSettings.notificationMinute
		)) {
			window->updating = true;
			navigationView.SelectedItem(navigationView.MenuItems().GetAt(1));
			window->updating = false;
			ContentDialog dialog;
			dialog.XamlRoot(XamlRoot());
			dialog.Style(
				Application::Current().Resources()
					.Lookup(winrt::box_value(L"DefaultContentDialogStyle"))
					.as<winrt::Microsoft::UI::Xaml::Style>()
			);
			RichTextBlock content;
			content.Blocks().Append(XamlReader::Load(
				resourceLoader.GetString(L"SettingsPage_Dialog_UnsavedEdit")
			).as<Paragraph>());
			content.IsTextSelectionEnabled(false);
			dialog.Content(content);
			dialog.PrimaryButtonText(resourceLoader.GetString(L"SettingsPage_Dialog_Save"));
			dialog.SecondaryButtonText(resourceLoader.GetString(L"SettingsPage_Dialog_Discard"));
			dialog.CloseButtonText(resourceLoader.GetString(L"SettingsPage_Dialog_Cancel"));
			dialog.DefaultButton(ContentDialogButton::Primary);
			auto response = co_await dialog.ShowAsync();
			if (response == ContentDialogResult::Primary) save();
			if (response == ContentDialogResult::None) co_return;
		}
		if (id == -2) {
			window->close();
		} else {
			window->updating = true;
			navigationView.SelectedItem(navigationView.MenuItems().GetAt(0));
			window->updating = false;
			window->MainFrame().Navigate(winrt::xaml_typename<winrt::Spacious::ListPage>());
			if (id != -1) window->MainFrame().Content().as<ListPage>()->editReminderByID(id);
		}
	}
	
	#undef check
}