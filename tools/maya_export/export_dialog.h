#pragma once

class xMeshSystem;

class ExportDialog {
public:
	ExportDialog(HWND hwndParent, xMeshSystem* object);
	~ExportDialog();

	BOOL init(HWND hwnd);
	void end(bool ok);
	int show();

	void setObjectName(std::string name);
	std::string getObjectName();
	void setObjectPath(std::string path);
	std::string getObjectPath();
	void setObjectExport(bool checked);
	bool getObjectExport();
	void clearMeshTabs();
	void addMeshTab(int index, std::string name);
	void updateMeshTab(int index, std::string name);
	void selectMeshTab(int index);
	void meshTabChange(bool update = true);
	void setMeshName(std::string text);
	std::string getMeshName();
	void setMeshPath(std::string text);
	std::string getMeshPath();
	void setMeshMaterial(std::string text);
	std::string getMeshMaterial();
	void setMeshExport(bool checked);
	bool getMeshExport();
	void setExportNormals(bool checked);
	bool getExportNormals();
	void setExportColors(bool checked);
	bool getExportColors();
	void setExportUVs(bool checked);
	bool getExportUVs();
	void updateMeshNames();
	void objectNameChange();
	void objectPathChange();

	
	HWND parent;
	HWND hwnd;
	xMeshSystem* object;
	int current_mesh;
};

BOOL CALLBACK ExportDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);