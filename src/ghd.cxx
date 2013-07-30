#include <isl/PidFile.hxx>
#include <isl/Log.hxx>
#include <isl/DirectLogger.hxx>
#include <isl/StreamLogTarget.hxx>
#include "GreenHouseServer.hxx"
#include <iostream>

int main(int argc, char *argv[])
{
	//isl::daemonize();
	isl::PidFile pidFile("ghd.pid");
	isl::DirectLogger logger;
	isl::StreamLogTarget coutLogTarget(logger, std::cout);
	isl::Log::debug().connect(coutLogTarget);
	isl::Log::warning().connect(coutLogTarget);
	isl::Log::error().connect(coutLogTarget);
	GreenHouseServer srv(argc, argv);
	srv.run();
}
