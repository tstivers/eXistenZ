#include "stdafx.h"
#include "resource.h"
#include "export_dialog.h"
#include "exportstatic.h"
#include "tinyxml/tinyxml.h"

static ExportDialog* dialog;

ExportDialog::ExportDialog(HWND parent, xMeshSystem* object)
{
	this->parent = parent;
	this->object = object;
	dialog = this;
}

ExportDialog::~ExportDialog()
{
}

void ExportDialog::setObjectName(std::string name)
{
	SetDlgItemText(hwnd, IDC_OBJECTNAME, name.c_str());
}

std::string ExportDialog::getObjectName()
{
	char buf[MAX_PATH];
	GetDlgItemText(hwnd, IDC_OBJECTNAME, buf, MAX_PATH);	
	return std::string(buf);
}

void ExportDialog::setObjectPath(std::string path)
{
	SetDlgItemText(hwnd, IDC_OBJECTPATH, path.c_str());
}

std::string ExportDialog::getObjectPath()
{
	char buf[MAX_PATH];
	GetDlgItemText(hwnd, IDC_OBJECTPATH, buf, MAX_PATH);
	return std::string(buf);
}

void ExportDialog::setMeshName(std::string name)
{
	SetDlgItemText(hwnd, IDC_MESHNAME, name.c_str());
}

std::string ExportDialog::getMeshName()
{
	char buf[MAX_PATH];
	GetDlgItemText(hwnd, IDC_MESHNAME, buf, MAX_PATH);
	return std::string(buf);
}

void ExportDialog::setMeshPath(std::string path)
{
	SetDlgItemText(hwnd, IDC_MESHPATH, path.c_str());
}

std::string ExportDialog::getMeshPath()
{
	char buf[MAX_PATH];
	GetDlgItemText(hwnd, IDC_MESHPATH, buf, MAX_PATH);
	return std::string(buf);
}

void ExportDialog::setMeshMaterial(std::string text)
{
	SetDlgItemText(hwnd, IDC_MATERIAL, text.c_str());
}

std::string ExportDialog::getMeshMaterial()
{
	char buf[MAX_PATH];
	GetDlgItemText(hwnd, IDC_MATERIAL, buf, MAX_PATH);
	return std::string(buf);
}

void ExportDialog::setObjectExport(bool checked)
{
	CheckDlgButton(hwnd, IDC_EXPORTOBJECT, checked ? BST_CHECKED : BST_UNCHECKED);
}

bool ExportDialog::getObjectExport()
{
	unsigned int checked = IsDlgButtonChecked(hwnd, IDC_EXPORTOBJECT);
	return checked == BST_CHECKED;
}

void ExportDialog::setMeshExport(bool checked)
{
	CheckDlgButton(hwnd, IDC_EXPORTMESH, checked ? BST_CHECKED : BST_UNCHECKED);
}

bool ExportDialog::getMeshExport()
{
	unsigned int checked = IsDlgButtonChecked(hwnd, IDC_EXPORTMESH);
	return checked == BST_CHECKED;
}

void ExportDialog::setExportNormals(bool checked)
{
	CheckDlgButton(hwnd, IDC_EXPORTNORMALS, checked ? BST_CHECKED : BST_UNCHECKED);
}

bool ExportDialog::getExportNormals()
{
	unsigned int checked = IsDlgButtonChecked(hwnd, IDC_EXPORTNORMALS);
	return checked == BST_CHECKED;
}

void ExportDialog::setExportColors(bool checked)
{
	CheckDlgButton(hwnd, IDC_EXPORTCOLORS, checked ? BST_CHECKED : BST_UNCHECKED);
}

bool ExportDialog::getExportColors()
{
	unsigned int checked = IsDlgButtonChecked(hwnd, IDC_EXPORTCOLORS);
	return checked == BST_CHECKED;
}

void ExportDialog::setExportUVs(bool checked)
{
	CheckDlgButton(hwnd, IDC_EXPORTUVS, checked ? BST_CHECKED : BST_UNCHECKED);
}

bool ExportDialog::getExportUVs()
{
	unsigned int checked = IsDlgButtonChecked(hwnd, IDC_EXPORTUVS);
	return checked == BST_CHECKED;
}

void ExportDialog::clearMeshTabs()
{
	HWND tabHwnd = GetDlgItem(hwnd, IDC_MESHTABS);
	TabCtrl_DeleteAllItems(tabHwnd);
}

void ExportDialog::addMeshTab(int index, std::string name)
{
	TCITEM item;
	ZeroMemory(&item, sizeof(TCITEM));
	item.mask = TCIF_TEXT;
	item.pszText = (char*)name.c_str();
	HWND tabHwnd = GetDlgItem(hwnd, IDC_MESHTABS);
	TabCtrl_InsertItem(tabHwnd, index, &item);
}

void ExportDialog::selectMeshTab(int index)
{
	HWND tabHwnd = GetDlgItem(hwnd, IDC_MESHTABS);
	TabCtrl_SetCurFocus(tabHwnd, index);
	meshTabChange(false);
}

void ExportDialog::meshTabChange(bool update)
{
	HWND tabHwnd = GetDlgItem(hwnd, IDC_MESHTABS);
	int index = TabCtrl_GetCurSel(tabHwnd);

	if(update && (current_mesh != index)) {
		// update info
		object->meshes[current_mesh]->name = getMeshName();
		object->meshes[current_mesh]->filepath = getMeshPath();
		object->meshes[current_mesh]->material = getMeshMaterial();
		object->meshes[current_mesh]->export = getMeshExport();
		object->meshes[current_mesh]->export_normals = getExportNormals();
		object->meshes[current_mesh]->export_colors = getExportColors();
		object->meshes[current_mesh]->export_uvs = getExportUVs();

		// update tab name
		TCITEM item;
		ZeroMemory(&item, sizeof(TCITEM));
		item.mask = TCIF_TEXT;
		item.pszText = (char*)object->meshes[current_mesh]->name.c_str();
		TabCtrl_SetItem(tabHwnd, current_mesh, &item);
		//InvalidateRect(hwnd, NULL, TRUE);
	}

	// show new mesh		
	setMeshName(object->meshes[index]->name);
	setMeshPath(object->meshes[index]->filepath);
	setMeshMaterial(object->meshes[index]->material);
	setMeshExport(object->meshes[index]->export);
	setExportNormals(object->meshes[index]->export_normals);
	setExportColors(object->meshes[index]->export_colors);
	setExportUVs(object->meshes[index]->export_uvs);
	current_mesh = index;	
}

void ExportDialog::updateMeshNames()
{
	clearMeshTabs();
	for(unsigned i = 0; i < object->meshes.size(); i++) {
		if(object->meshes.size() == 1)
			object->meshes[i]->name = object->name;
		else {
			char buf[25];
			object->meshes[i]->name = object->name + "_" + itoa(i, buf, 25);
		}

		addMeshTab(i, object->meshes[i]->name);
	}

	selectMeshTab(current_mesh);
	//InvalidateRect(hwnd, NULL, TRUE);
}

void ExportDialog::objectNameChange()
{
	if(object->name != getObjectName()) {
		object->name = getObjectName();
		updateMeshNames();
	}
}

void ExportDialog::objectPathChange()
{
	if(object->filepath != getObjectPath()) {
		object->filepath = getObjectPath();
		for(unsigned i = 0; i < object->meshes.size(); i++) {
			object->meshes[i]->filepath = object->filepath;
		}

		setMeshPath(object->meshes[current_mesh]->filepath);
	}
}

int ExportDialog::show() 
{	
	return DialogBox(MhInstPlugin, MAKEINTRESOURCE(IDD_EXPORTSTATIC), parent, (DLGPROC)ExportDialogProc);		
}

BOOL ExportDialog::init(HWND hwnd) 
{
	this->hwnd = hwnd;

	setObjectName(object->name);
	setObjectPath(object->filepath);
	setObjectExport(object->export);

	// add tabs for meshes
	current_mesh = 0;
	updateMeshNames();

	return TRUE;
}

void ExportDialog::end(bool ok)
{
	// update the meshsystem
	object->name = getObjectName();
	object->filepath = getObjectPath();
	object->export = getObjectExport();
	
	// update the current mesh
	// TODO: fix this
	object->meshes[current_mesh]->name = getMeshName();
	object->meshes[current_mesh]->filepath = getMeshPath();
	object->meshes[current_mesh]->material = getMeshMaterial();
	object->meshes[current_mesh]->export = getMeshExport();
	object->meshes[current_mesh]->export_normals = getExportNormals();
	object->meshes[current_mesh]->export_colors = getExportColors();
	object->meshes[current_mesh]->export_uvs = getExportUVs();
}

BOOL CALLBACK ExportDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
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

		case IDC_OBJECTNAME:
			switch(HIWORD(wParam))
			{
			case EN_CHANGE:
				dialog->objectNameChange();
				return TRUE;
			default:
				return FALSE;
			}

		case IDC_OBJECTPATH:
			switch(HIWORD(wParam))
			{
			case EN_CHANGE:
				dialog->objectPathChange();
				return TRUE;
			default:
				return FALSE;
			}


		default:
			return FALSE;
		}

	case WM_NOTIFY:
		switch(wParam)
		{
		case IDC_MESHTABS:
			switch (((LPNMHDR) lParam)->code) 
			{ 
			case TCN_SELCHANGE:
				dialog->meshTabChange();
				return TRUE;
			default:
				return FALSE;
			}
		default:
			return FALSE;
		}

	default: 
		return FALSE; 
	} 
}
