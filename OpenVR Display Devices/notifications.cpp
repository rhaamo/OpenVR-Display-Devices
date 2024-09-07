#include "notifications.h"
#include <sapi.h>

//Webchaussette wsClient(XSOVERLAY_WS_HOST, XSOVERLAY_WS_PORT, XSOVERLAY_WS_QUERY);

void enableSoundNotifications() {
	if (!WinToastLib::WinToast::isCompatible()) {
		throw std::runtime_error("System incompatible :(");
	}
	WinToastLib::WinToast::instance()->setAppName(L"" OPENVR_APPLICATION_NAME);
	const auto aumi = WinToastLib::WinToast::configureAUMI(L"ottpossum", L"" OPENVR_APPLICATION_NAME, L"farts", L"42");
	WinToastLib::WinToast::instance()->setAppUserModelId(aumi);
	if (!WinToastLib::WinToast::instance()->initialize()) {
		throw std::runtime_error("Woops, error when initializing the WinToaster");
	}

	/*std::string title = "Test notification";
	std::string content = "nya~ :3";

	sendSoundNotifications(title, content);
	std::cout << "Enabled toaster notifications" << std::endl;*/
}

void disableSoundNotifications() {
	// Nothing to do here
}

void enableXsOverlayNotifications() {
	/*wsClient.Start();

	std::string title = "Test notification";
	std::string content = "nya~ :3";
	sendXsOverlayNotification(title, content);
	std::cout << "Enabled XSOverlay notifications" << std::endl;
	*/
}

std::string buildXsOverlayJson(std::string title, std::string content) {
	nlohmann::json jNotification = {
		{"title", title},
		{"content", content},
		{"timeout", 3},
		{"height", "150"},
		{"opacity", 1},
		{"volume", 1},
		{"audioPath", "default"},
		{"icon", nullptr},
		{"sourceApp", ""},
		{"type", 1},
		{"useBase64Icon", false}
	};
	nlohmann::json jWrapper = {
		{"sender", "OVRDisplayDevices"},
		{"target", "xsoverlay"},
		{"command", "SendNotification"},
		{"jsonData", jNotification.dump()},
		{"rawData", nullptr}
	};
	return jWrapper.dump();
}

void disableXsOverlayNotifications() {
	//wsClient.Stop();
}

std::wstring charToWstring(const char *src) {
	return std::wstring(src, src + strlen(src));
}

wchar_t *CharToWchar(const char *str) {
	int lenW, lenA = lstrlenA(str);
	wchar_t *unicodestr;

	lenW = MultiByteToWideChar(CP_ACP, 0, str, lenA, NULL, 0);
	if (lenW > 0) {
		unicodestr = SysAllocStringLen(0, lenW);
		MultiByteToWideChar(CP_ACP, 0, str, lenA, unicodestr, lenW);
		return unicodestr;
	}
	return NULL;
}

void sendSoundNotifications(std::string &title, std::string &msg) {
	/*WinToastLib::WinToastTemplate::AudioOption audioOptions = WinToastLib::WinToastTemplate::AudioOption::Default;

	WinToastLib::WinToastTemplate templ(WinToastLib::WinToastTemplate::Text02);
	// Title
	templ.setTextField(charToWstring(title.c_str()), WinToastLib::WinToastTemplate::FirstLine);
	// Content
	templ.setAttributionText(charToWstring(msg.c_str()));
	templ.setAudioOption(audioOptions);
	// templ.setExpiration(5);
	if (WinToastLib::WinToast::instance()->showToast(templ, new CustomHandler()) < 0) {
		throw std::runtime_error("Woops, cannot toast :(");
	}*/

	ISpVoice *pVoice = NULL;

	/*if (FAILED(::CoInitialize(NULL))) {
		std::cout << "SAPI failed to initialize." << std::endl;
		return;
	}*/

	//std::string speakText = title + "\n\n" + msg;
	std::string speakText = title;

	HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
	if (SUCCEEDED(hr)) {
		//hr = pVoice->Speak(L"Hello world", 0, NULL);
		//hr = pVoice->Speak(speakText.c_str(), 0, NULL);

		/*wchar_t *wtext;
		wtext = malloc(speakText.length() * sizeof(wchar_t));
		mbstowcs(wtext, speakText.c_str(), speakText.length());//includes null
		LPWSTR ptr = wtext;
		hr = pVoice->Speak(ptr, 0, NULL);
		free(wtext);
		*/
		wchar_t *text = CharToWchar(speakText.c_str());
		hr = pVoice->Speak(text, 0, NULL);

		pVoice->Release();
		pVoice = NULL;
	}
	else
	{
		std::cout << "SAPI failed to create instance." << std::endl;
	}
}

void sendXsOverlayNotification(std::string &title, std::string &msg) {
	std::string jmsg = buildXsOverlayJson(title, msg);
	//wsClient.Send(jmsg);
}

void sendNotification(std::string &title, std::string &msg) {
	if (application_configuration.notificationsSound) {
		std::cout << "SN: " << title << std::endl;
		sendSoundNotifications(title, msg);
	}
	if (application_configuration.notificationsXsOverlay) {
		sendXsOverlayNotification(title, msg);
	}
}
