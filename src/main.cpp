#include <iostream>

#pragma warning(push, 0)
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>
#include <FL/x.H>
#pragma warning(pop)

#include "version.h"
#include "preferences.h"
#include "themes.h"
#include "main-window.h"

#ifdef _WIN32

#include <shlobj.h>
#include <tchar.h>

#define MAKE_WSTR_HELPER(x) L ## x
#define MAKE_WSTR(x) MAKE_WSTR_HELPER(x)

#elif defined(__APPLE__)
#include "cocoa.h"
#endif

static Main_Window *window = nullptr;

void open_dragged_cb(const char *filename) {
	if (window) {
		window->drag_and_drop_tilemap(filename);
	}
}

static void use_theme(OS::Theme theme) {
	OS::use_native_fonts();
	switch (theme) {
	case OS::Theme::CLASSIC:
		OS::use_classic_theme();
		return;
	case OS::Theme::AERO:
		OS::use_aero_theme();
		return;
	case OS::Theme::METRO:
		OS::use_metro_theme();
		return;
	case OS::Theme::AQUA:
		OS::use_aqua_theme();
		return;
	case OS::Theme::GREYBIRD:
		OS::use_greybird_theme();
		return;
	case OS::Theme::OCEAN:
		OS::use_ocean_theme();
		return;
	case OS::Theme::BLUE:
		OS::use_blue_theme();
		return;
	case OS::Theme::OLIVE:
		OS::use_olive_theme();
		return;
	case OS::Theme::ROSE_GOLD:
		OS::use_rose_gold_theme();
		return;
	case OS::Theme::DARK:
		OS::use_dark_theme();
		return;
	case OS::Theme::BRUSHED_METAL:
		OS::use_brushed_metal_theme();
		return;
	case OS::Theme::HIGH_CONTRAST:
		OS::use_high_contrast_theme();
	}
}

int main(int argc, char **argv) {
	Preferences::initialize(argv[0]);
	std::ios::sync_with_stdio(false);
#ifdef _WIN32
	SetCurrentProcessExplicitAppUserModelID(MAKE_WSTR(PROGRAM_AUTHOR) L"." MAKE_WSTR(PROGRAM_NAME));
#endif
	Fl::visual(FL_DOUBLE | FL_RGB);

#ifdef _WIN32
	OS::Theme default_theme = OS::Theme::BLUE;
	DWORD reg_value = 1, reg_size = sizeof(reg_value);
	if (!RegGetValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize"),
		_T("AppsUseLightTheme"), RRF_RT_REG_DWORD, NULL, &reg_value, &reg_size) && reg_value == 0) {
		default_theme = OS::Theme::DARK;
	}
	OS::Theme theme = (OS::Theme)Preferences::get("theme", (int)default_theme);
#elif defined(__APPLE__)
	OS::Theme default_theme = OS::Theme::AQUA;
	if (cocoa_is_dark_mode()) default_theme = OS::Theme::DARK;
	OS::Theme theme = (OS::Theme)Preferences::get("theme", (int)default_theme);
#else
	OS::Theme theme = (OS::Theme)Preferences::get("theme", (int)OS::Theme::GREYBIRD);
#endif
	use_theme(theme);

#ifdef _WIN32
	int x = Preferences::get("x", 48), y = Preferences::get("y", 48 + GetSystemMetrics(SM_CYCAPTION));
#else
	int x = Preferences::get("x", 48), y = Preferences::get("y", 48);
#endif
	int w = Preferences::get("w", 647), h = Preferences::get("h", 406);
	window = new Main_Window(x, y, w, h);
	window->show();
	OS::update_macos_appearance(window);
	if (window->transparent()) {
		window->apply_transparency();
	}
	if (window->full_screen()) {
		window->fullscreen();
	}
	else if (Preferences::get("maximized")) {
		window->maximize();
	}

	int argi = 1;
#ifdef __APPLE__
	// Ignore the "-psn_*" parameter passed by some older macOS versions
	// See https://stackoverflow.com/questions/10242115/os-x-strange-psn-command-line-parameter-when-launched-from-finder
	while (argi < argc) {
		if (memcmp(argv[argi], "-psn_", 4) != 0) break;
		argi++;
	}
#endif

	if (argc - argi >= 2) {
		window->open_tilemap(argv[argi+0]);
		window->load_tileset(argv[argi+1]);
	}
	else if (argc - argi >= 1) {
		window->open_or_import_or_convert(argv[argi]);
	}
	fl_open_callback(open_dragged_cb);

	return Fl::run();
}
