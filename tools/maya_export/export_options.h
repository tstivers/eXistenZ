#pragma once

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

class exportStatic: public MPxCommand
{
public:
	exportStatic();
	virtual	~exportStatic();
	static void* creator();
	virtual MStatus	doIt(const MArgList& args);

private:
	MStatus status;
	void exportMesh(const MDagPath& dagPath);
	MIntArray GetLocalIndex( MIntArray & getVertices, MIntArray & getTriangle );
	std::string GetTexture(MObject& shader);
	xMeshSystem meshsystem;
};

BOOL CALLBACK OptionsDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

extern Options options;