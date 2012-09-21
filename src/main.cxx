#include <isl/common.hxx>
#include <isl/FileLogTarget.hxx>
#include "GreenHouseServer.hxx"

int main(int argc, char *argv[])
{
	//isl::daemonize();
	isl::writePid("ghd.pid");
	isl::debugLog().connectTarget(isl::FileLogTarget("ghd.log"));
	isl::warningLog().connectTarget(isl::FileLogTarget("ghd.log"));
	isl::errorLog().connectTarget(isl::FileLogTarget("ghd.log"));
	GreenHouseServer srv(argc, argv);
	srv.run();
	isl::debugLog().disconnectTargets();
	isl::warningLog().disconnectTargets();
	isl::errorLog().disconnectTargets();
}
