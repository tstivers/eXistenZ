#pragma once

class scanDag: public MPxCommand
{
public:
	scanDag() {};
	virtual			~scanDag();
	static void*	creator();
	virtual MStatus	doIt( const MArgList& );

private:
	MStatus	parseArgs( const MArgList& args, MItDag::TraversalType& traversalType, MFn::Type& filter, bool& quiet);
	MStatus	doScan( const MItDag::TraversalType traversalType, MFn::Type filter, bool quiet);
	void printTransformData(const MDagPath& dagPath, bool quiet);
};
