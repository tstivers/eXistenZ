#include "stdafx.h"
#include "scandag.h"

scanDag::~scanDag() {}

void* scanDag::creator()
{
	return new scanDag;
}

MStatus	scanDag::doIt( const MArgList& args )
{
	MItDag::TraversalType	traversalType = MItDag::kDepthFirst;
	MFn::Type				filter        = MFn::kInvalid;
	MStatus					status;
	bool					quiet = false;

	status = parseArgs ( args, traversalType, filter, quiet );
	if (!status)
		return status;

	return doScan( traversalType, filter, quiet);
};

MStatus scanDag::parseArgs( const MArgList& args,
						   MItDag::TraversalType& traversalType,
						   MFn::Type& filter,
						   bool & quiet)
{
	MStatus     	stat;
	MString     	arg;
	const MString	breadthFlag				("-b");
	const MString	breadthFlagLong			("-breadthFirst");
	const MString	depthFlag				("-d");
	const MString	depthFlagLong			("-depthFirst");
	const MString	cameraFlag				("-c");
	const MString	cameraFlagLong			("-cameras");
	const MString	lightFlag				("-l");
	const MString	lightFlagLong			("-lights");
	const MString	nurbsSurfaceFlag		("-n");
	const MString	nurbsSurfaceFlagLong	("-nurbsSurfaces");
	const MString	quietFlag				("-q");
	const MString	quietFlagLong			("-quiet");

	// Parse the arguments.
	for ( unsigned int i = 0; i < args.length(); i++ ) {
		arg = args.asString( i, &stat );
		if (!stat)              
			continue;

		if ( arg == breadthFlag || arg == breadthFlagLong )
			traversalType = MItDag::kBreadthFirst;
		else if ( arg == depthFlag || arg == depthFlagLong )
			traversalType = MItDag::kDepthFirst;
		else if ( arg == cameraFlag || arg == cameraFlagLong )
			filter = MFn::kCamera;
		else if ( arg == lightFlag || arg == lightFlagLong )
			filter = MFn::kLight;
		else if ( arg == nurbsSurfaceFlag || arg == nurbsSurfaceFlagLong )
			filter = MFn::kNurbsSurface;
		else if ( arg == quietFlag || arg == quietFlagLong )
			quiet = true;
		else {
			arg += ": unknown argument";
			displayError(arg);
			return MS::kFailure;
		}
	}
	return stat;
}

MStatus scanDag::doScan( const MItDag::TraversalType traversalType,
						MFn::Type filter,
						bool quiet)
{   
	MStatus status;

	MItDag dagIterator( traversalType, filter, &status);

	if ( !status) {
		status.perror("MItDag constructor");
		return status;
	}

	//	Scan the entire DAG and output the name and depth of each node

	if (traversalType == MItDag::kBreadthFirst)
		if (!quiet)
			std::cout << std::endl << "Starting Breadth First scan of the Dag";
		else
			if (!quiet)
				std::cout << std::endl << "Starting Depth First scan of the Dag";

	switch (filter) {
		case MFn::kCamera:
			if (!quiet)
				std::cout << ": Filtering for Cameras\n";
			break;
		case MFn::kLight:
			if (!quiet)
				std::cout << ": Filtering for Lights\n";
			break;
		case MFn::kNurbsSurface:
			if (!quiet)
				std::cout << ": Filtering for Nurbs Surfaces\n";
			break;
		default:
			std::cout << std::endl;
	}

	int objectCount = 0;
	for ( ; !dagIterator.isDone(); dagIterator.next() ) {

		MDagPath dagPath;

		status = dagIterator.getPath(dagPath);
		if ( !status ) {
			status.perror("MItDag::getPath");
			continue;
		}

		MFnDagNode dagNode(dagPath, &status);
		if ( !status ) {
			status.perror("MFnDagNode constructor");
			continue;
		}

		if (!quiet)
			std::cout << dagNode.name() << ": " << dagNode.typeName() << std::endl;

		if (!quiet)
			std::cout << "  dagPath: " << dagPath.fullPathName() << std::endl;

		objectCount += 1;
		if (dagPath.hasFn(MFn::kCamera)) {
			MFnCamera camera (dagPath, &status);
			if ( !status ) {
				status.perror("MFnCamera constructor");
				continue;
			}

			// Get the translation/rotation/scale data
			printTransformData(dagPath, quiet);

			// Extract some interesting Camera data
			if (!quiet)
			{
				std::cout << "  eyePoint: "
					<< camera.eyePoint(MSpace::kWorld) << std::endl;
				std::cout << "  upDirection: "
					<< camera.upDirection(MSpace::kWorld) << std::endl;
				std::cout << "  viewDirection: "
					<< camera.viewDirection(MSpace::kWorld) << std::endl;
				std::cout << "  aspectRatio: " << camera.aspectRatio() << std::endl;
				std::cout << "  horizontalFilmAperture: "
					<< camera.horizontalFilmAperture() << std::endl;
				std::cout << "  verticalFilmAperture: "
					<< camera.verticalFilmAperture() << std::endl;
			}
		} else if (dagPath.hasFn(MFn::kLight)) {
			MFnLight light (dagPath, &status);
			if ( !status ) {
				status.perror("MFnLight constructor");
				continue;
			}

			// Get the translation/rotation/scale data
			printTransformData(dagPath, quiet);

			// Extract some interesting Light data
			MColor color;

			color = light.color();
			if (!quiet)
			{
				std::cout << "  color: ["
					<< color.r << ", "
					<< color.g << ", "
					<< color.b << "]\n";
			}
			color = light.shadowColor();
			if (!quiet)
			{
				std::cout << "  shadowColor: ["
					<< color.r << ", "
					<< color.g << ", "
					<< color.b << "]\n";

				std::cout << "  intensity: " << light.intensity() << std::endl;
			}
		} else if (dagPath.hasFn(MFn::kNurbsSurface)) {
			MFnNurbsSurface surface (dagPath, &status);
			if ( !status ) {
				status.perror("MFnNurbsSurface constructor");
				continue;
			}

			// Get the translation/rotation/scale data
			printTransformData(dagPath, quiet);

			// Extract some interesting Surface data
			if (!quiet)
			{
				std::cout << "  numCVs: "
					<< surface.numCVsInU()
					<< " * "
					<< surface.numCVsInV()
					<< std::endl;
				std::cout << "  numKnots: "
					<< surface.numKnotsInU()
					<< " * "
					<< surface.numKnotsInV()
					<< std::endl;
				std::cout << "  numSpans: "
					<< surface.numSpansInU()
					<< " * "
					<< surface.numSpansInV()
					<< std::endl;
			}
		} else {
			// Get the translation/rotation/scale data
			printTransformData(dagPath, quiet);
		} 
	}

	if (!quiet)
	{
		std::cout.flush();
	}
	setResult(objectCount);
	return MS::kSuccess;
}

void scanDag::printTransformData(const MDagPath& dagPath, bool quiet)
{
	MStatus		status;
	MObject		transformNode = dagPath.transform(&status);
	// This node has no transform - i.e., it's the world node
	if (!status && status.statusCode () == MStatus::kInvalidParameter)
		return;
	MFnDagNode	transform (transformNode, &status);
	if (!status) {
		status.perror("MFnDagNode constructor");
		return;
	}
	MTransformationMatrix	matrix (transform.transformationMatrix());

	if (!quiet)
	{
		std::cout << "  translation: " << matrix.translation(MSpace::kWorld)
			<< std::endl;
	}
	double									threeDoubles[3];
	MTransformationMatrix::RotationOrder	rOrder;

	matrix.getRotation (threeDoubles, rOrder, MSpace::kWorld);
	if (!quiet)
	{
		std::cout << "  rotation: ["
			<< threeDoubles[0] << ", "
			<< threeDoubles[1] << ", "
			<< threeDoubles[2] << "]\n";
	}
	matrix.getScale (threeDoubles, MSpace::kWorld);
	if (!quiet)
	{
		std::cout << "  scale: ["
			<< threeDoubles[0] << ", "
			<< threeDoubles[1] << ", "
			<< threeDoubles[2] << "]\n";
	}
}