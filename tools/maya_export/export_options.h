#pragma once

#define MESH_EXTENSION ".tm"
#define MESHSYSTEM_EXTENSION ".tms"

class Options {
public:
	Options();
	~Options();

	void loadOptions();
	void saveOptions();

	std::string export_path;
	std::string mat_strip_path;
	bool allow_multi;
	bool combine_meshes;
	bool optimize_meshes;
};

class OptionsDialog {
public:
	OptionsDialog(HWND parent);
	~OptionsDialog();

	BOOL init(HWND hwnd);
	void end(bool ok);
	int show();

	HWND parent;
	HWND hwnd;
};

class optionsCmd: public MPxCommand
{
public:
	optionsCmd() {};
	virtual	~optionsCmd() {};
	static void* creator() { return new optionsCmd(); }
	virtual MStatus	doIt(const MArgList& args);
};

BOOL CALLBACK OptionsDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

extern Options options;