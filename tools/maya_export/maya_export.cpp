#include "stdafx.h"
#include "maya/MFnPlugin.h"
#include "maya_export.h"
#include "scandag.h"
#include "exportStatic.h"
#include "export_options.h"

Options options;

MStatus initializePlugin( MObject obj )
{ 
	MStatus status;
	
	MFnPlugin plugin (obj, "Eyetraxx eXistenZ", "1.0", "Any");
	plugin.setName("eXistenZ Engine Plugin");
	plugin.registerCommand("exScanDag", scanDag::creator);
	plugin.registerCommand("exExportStatic", exportStatic::creator);
	plugin.registerCommand("exOptions", optionsCmd::creator);	
	options.loadOptions();

	// set up menu
	MString result;
	MGlobal::executeCommand("setParent \"MayaWindow\";", result);
	MGlobal::executeCommand("menu -p \"MayaWindow\" -label \"eXistenZ\" -tearOff false eXistenZ;", result);
	MGlobal::executeCommand("menuItem -label \"Export Selected\" -command exExportStatic;", result);
	MGlobal::executeCommand("menuItem -label \"Options\" -command exOptions;", result);	

	return status;
}

MStatus uninitializePlugin( MObject obj )
{
	MStatus status;

	MFnPlugin plugin(obj);
	plugin.deregisterCommand("exScanDag");
	plugin.deregisterCommand("exExportStatic");
	plugin.deregisterCommand("exOptions");
	options.saveOptions();

	MString result;	
	MGlobal::executeCommand("deleteUI eXistenZ;", result);

	return status;
}
