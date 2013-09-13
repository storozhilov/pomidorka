#ifndef HTTP_SERVICE__HXX
#define HTTP_SERVICE__HXX 1

#include <isl/AbstractSyncTcpService.hxx>
#include <isl/HttpRequestReader.hxx>
#include <isl/HttpResponseStreamWriter.hxx>
#include "AbstractLightingProgram.hxx"

class HttpService : public isl::AbstractSyncTcpService
{
public:
        HttpService(Subsystem * owner, AbstractLightingProgram& lightingProgram, int listenPort, size_t maxClients,
                        const isl::Timeout& dataTransmissionTimeout) :
                isl::AbstractSyncTcpService(owner, maxClients),
                _lightingProgram(lightingProgram),
                _dataTransmissionTimeout(dataTransmissionTimeout)
        {
                addListener(isl::TcpAddrInfo(isl::TcpAddrInfo::IpV4, isl::TcpAddrInfo::WildcardAddress, listenPort));
        }
private:
        class HttpTask : public AbstractTask
        {
        public:
                HttpTask(HttpService& service, isl::TcpSocket& socket) :
                        AbstractTask(socket),
                        _service(service)
                {}
        private:
                HttpTask();

                //! Task execution method definition
                virtual void executeImpl(TaskDispatcherType& taskDispatcher)
                {
                        // Reading HTTP-response
                        isl::HttpRequestParser parser;
                        isl::HttpRequestReader reader(parser);
			bool requestFetched = false;
			try {
				size_t bytesReadFromDevice;
				requestFetched = reader.read(socket(), isl::Timestamp::limit(isl::Timeout(_service._dataTransmissionTimeout)), &bytesReadFromDevice);
				if (requestFetched) {
					isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Request has been fetched, bytesReadFromDevice = ") << bytesReadFromDevice);
				} else {
					isl::Log::warning().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Request have NOT been fetched, bytesReadFromDevice = ") << bytesReadFromDevice);
					return;
				}
			} catch (std::exception& e) {
				isl::Log::error().log(isl::ExceptionLogMessage(SOURCE_LOCATION_ARGS, e));
				return;
			} catch (...) {
				isl::Log::error().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Unknown error occured"));
				return;
			}

			// Composing an HTTP-response
			std::ostringstream oss;
			oss << "<html><head><title>Greenhouse server WEB-console</title></head><meta http-equiv=\"refresh\" content=\"1\" /><body>";
			if (!requestFetched) {
				if (parser.isBad()) {
                                        // TODO: HTTP-response status support
					oss << "<p>Bad request: &quot;" << parser.error()->message() << "&quot;</p>";
				} else {
                                        // TODO: HTTP-response status support
					oss << "<p>Timeout expired</p>";
				}
			} else {
                                // Requesting state from the lighting SCADA program
                                std::auto_ptr<AbstractLightingProgram::State> stateAutoPtr(_service._lightingProgram.requestState());

                                oss << "<table><thead><tr><th colspan=\"2\">Lighting control subsystem</th></tr></thead><tbody>";

                                if (stateAutoPtr.get()) {
                                        oss << "<tr><td><strong>Status register value:</strong></td><td>" << stateAutoPtr->statusRegisterValue << "</td></tr>";
                                        oss << "<tr><td><strong>Light sensor register value:</strong></td><td>" << stateAutoPtr->lightSensorRegisterValue << "</td></tr>";
                                        oss << "<tr><td><strong>Lamp supply level register value:</strong></td><td>" << stateAutoPtr->lampSupplyLevelRegisterValue << "</td></tr>";
                                        oss << "<tr><td><strong>Current light intencity, %:</strong></td><td>" << stateAutoPtr->currentLightIntensity << "</td></tr>";
                                        oss << "<tr><td><strong>Desired light intencity, %:</strong></td><td>" << stateAutoPtr->desiredLightIntensity << "</td></tr>";
                                } else {
                                        oss << "<tr><td colspan=\"2\">No response from the lighting subsystem</td></tr>";
                                }
                                oss << "</tbody></table>";
			}
			oss << "</body></html>";
			// Sending an HTTP-response to the client
			isl::HttpResponseStreamWriter responseWriter;
			responseWriter.setHeaderField("Content-Type", "text/html; charset=utf-8");
			responseWriter.writeOnce(socket(), oss.str(), isl::Timestamp::limit(isl::Timeout(_service._dataTransmissionTimeout)));
                }

                HttpService& _service;
        };
        //! Task creation factory method definition
        virtual AbstractTask * createTask(isl::TcpSocket& socket)
        {
                return new HttpTask(*this, socket);
        }

        AbstractLightingProgram& _lightingProgram;
        isl::Timeout _dataTransmissionTimeout;
};

#endif
