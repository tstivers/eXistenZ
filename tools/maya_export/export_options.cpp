#include "stdafx.h"
#include "resource.h"
#include "export_options.h"

static OptionsDialog* dialog;

Options::Options()
{
	export_path = "";
	mat_strip_path = "";
	allow_multi = true;
	combine_meshes = true;
	optimize_meshes = true;
}

Options::~Options()
{
}

void Options::loadOptions()
{
	HKEY key;
	char buf[MAX_PATH];
	DWORD bbuf;
	DWORD bufsize;

	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\eXistenZ\\Maya", 0, KEY_QUERY_VALUE, &key)) {
		
		// read key values
		bufsize = MAX_PATH;
		if(ERROR_SUCCESS == RegQueryValueEx( key, "ExportPath", NULL, NULL,(LPBYTE)buf, &bufsize))
			export_path = buf;

		bufsize = MAX_PATH;
		if(ERROR_SUCCESS == RegQueryValueEx( key, "MaterialStripPath", NULL, NULL,(LPBYTE)buf, &bufsize))
			mat_strip_path = buf;

		bufsize = MAX_PATH;
		if(ERROR_SUCCESS == RegQueryValueEx( key, "AllowMulti", NULL, NULL,(LPBYTE)&bbuf, &bufsize))
			allow_multi = (bbuf == 1);

		bufsize = MAX_PATH;
		if(ERROR_SUCCESS == RegQueryValueEx( key, "CombineMeshes", NULL, NULL,(LPBYTE)&bbuf, &bufsize))
			combine_meshes = (bbuf == 1);

		bufsize = MAX_PATH;
		if(ERROR_SUCCESS == RegQueryValueEx( key, "OptimizeMeshes", NULL, NULL,(LPBYTE)&bbuf, &bufsize))
			optimize_meshes = (bbuf == 1);
	}

	RegCloseKey(key);
}

void Options::saveOptions()
{
	HKEY key;
	DWORD bleh;

	if(ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\eXistenZ\\Maya", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &bleh))
		return;

	RegSetValueEx(key, "ExportPath", 0, REG_SZ, (const byte*)export_path.c_str(), export_path.size());
	RegSetValueEx(key, "MaterialStripPath", 0, REG_SZ, (const byte*)mat_strip_path.c_str(), mat_strip_path.size());

	bleh = allow_multi ? 1 : 0;
	RegSetValueEx(key, "AllowMulti", 0, REG_DWORD, (const BYTE*)&bleh, sizeof(DWORD));
	bleh = combine_meshes ? 1 : 0;
	RegSetValueEx(key, "CombineMeshes", 0, REG_DWORD, (const BYTE*)&bleh, sizeof(DWORD));
	bleh = optimize_meshes ? 1 : 0;
	RegSetValueEx(key, "OptimizeMeshes", 0, REG_DWORD, (const BYTE*)&bleh, sizeof(DWORD));

	RegCloseKey(key);
}

OptionsDialog::OptionsDialog(HWND parent)
{
	this->parent = parent;
	dialog = this;
}

OptionsDialog::~OptionsDialog()
{
}

BOOL OptionsDialog::init(HWND hwnd)
{
	this->hwnd = hwnd;
	SetDlgItemText(hwnd, IDC_EXPORTPATH, options.export_path.c_str());
	SetDlgItemText(hwnd, IDC_STRIPPATH, options.mat_strip_path.c_str());
	CheckDlgButton(hwnd, IDC_ALLOWMULTI, options.allow_multi ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_COMBINEMESHES, options.combine_meshes ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_OPTIMIZE, options.optimize_meshes ? BST_CHECKED : BST_UNCHECKED);

	return TRUE;
}

void OptionsDialog::end(bool ok)
{
	char buf[MAX_PATH];
	if(ok) {
		GetDlgItemText(hwnd, IDC_EXPORTPATH, buf, MAX_PATH);
		options.export_path = buf;
		GetDlgItemText(hwnd, IDC_STRIPPATH, buf, MAX_PATH);
		options.mat_strip_path = buf;
		options.allow_multi = (IsDlgButtonChecked(hwnd, IDC_ALLOWMULTI) == BST_CHECKED);
		options.combine_meshes = (IsDlgButtonChecked(hwnd, IDC_COMBINEMESHES) == BST_CHECKED);
		options.optimize_meshes = (IsDlgButtonChecked(hwnd, IDC_OPTIMIZE) == BST_CHECKED);
		options.saveOptions();
	}
}

int OptionsDialog::show()
{
	return DialogBox(MhInstPlugin, MAKEINTRESOURCE(IDD_EXPORTOPTIONS), parent, (DLGPROC)OptionsDialogProc);
}

BOOL CALLBACK OptionsDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
	switch (message) 
	{ 
	case WM_INITDIALOG:
		return dialog->init(hwndDlg);

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			dialog->end(true);
			EndDialog(hwndDlg, 1);
			return TRUE;

		case IDCANCEL:
			dialog->end(false);
			EndDialog(hwndDlg, 0);
			return TRUE;

		default:
			return FALSE;
		}

	default: 
		return FALSE; 
	} 
}

MStatus optionsCmd::doIt(const MArgList& args)
{
	OptionsDialog options_dialog(M3dView::applicationShell());
	options_dialog.show();

	MStatus status;
	return status;
}