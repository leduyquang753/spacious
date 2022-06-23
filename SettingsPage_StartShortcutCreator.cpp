#include "pch.h"

#include <objbase.h>
#include <propkey.h>
#include <propvarutil.h>
#include <ShObjIdl_core.h>
#include <wrl/client.h>

#include "SettingsPage.xaml.h"

using namespace Microsoft::WRL;
using namespace winrt::Microsoft::UI::Xaml;

namespace winrt::Spacious::implementation {
	#define check() winrt::check_hresult(r)
	
	void SettingsPage::onCreateStartShortcut(const IInspectable &source, const RoutedEventArgs &args) {
		std::wstring shortcutPath(
			GetEnvironmentVariable(L"AppData", nullptr, 0), 0
		);
		GetEnvironmentVariable(
			L"AppData", shortcutPath.data(),
			static_cast<DWORD>(shortcutPath.size())
		);
		shortcutPath.pop_back(); // Get rid of the null terminator.
		shortcutPath += L"\\Microsoft\\Windows\\Start Menu\\Programs\\Spacious.lnk";

		HRESULT r;

		DWORD attributes = GetFileAttributes(shortcutPath.c_str());
		bool fileExists = attributes < 0xFFFFFFF;
		if (fileExists) return;

		wchar_t *programPath;
		_get_wpgmptr(&programPath);
		ComPtr<IShellLink> shellLink;
		r = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shellLink));
		check();
		r = shellLink->SetPath(programPath);
		check();
		r = shellLink->SetArguments(L"");
		check();
		ComPtr<IPropertyStore> propertyStore;
		r = shellLink.As(&propertyStore);
		check();
		PROPVARIANT appIdPropVar;
		r = InitPropVariantFromString(L"cf.leduyquang753.spacious", &appIdPropVar);
		check();
		r = propertyStore->SetValue(PKEY_AppUserModel_ID, appIdPropVar);
		check();
		r = InitPropVariantFromString(L"{e6cb0677-53b6-4a0d-810a-e67e9a398446}", &appIdPropVar);
		check();
		r = propertyStore->SetValue(PKEY_AppUserModel_ToastActivatorCLSID, appIdPropVar);
		check();
		r = propertyStore->Commit();
		check();
		ComPtr<IPersistFile> persistFile;
		r = shellLink.As(&persistFile);
		check();
		r = persistFile->Save(shortcutPath.c_str(), TRUE);
		check();
		PropVariantClear(&appIdPropVar);
	}

	#undef check
}