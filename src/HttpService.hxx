#ifndef HTTP_SERVICE__HXX
#define HTTP_SERVICE__HXX 1

#include <isl/AbstractSyncTcpService.hxx>
#include <isl/HttpRequestReader.hxx>
#include <isl/HttpResponseStreamWriter.hxx>
#include "LightingSubsystem.hxx"
#include <fstream>

#define INDEX_PATH "/index.html"

class HttpService : public isl::AbstractSyncTcpService
{
public:
        HttpService(Subsystem * owner, LightingSubsystem& LightingSubsystem, size_t maxClients, const std::string& htdocsDir,
                        const isl::Timeout& dataTransmissionTimeout) :
                isl::AbstractSyncTcpService(owner, maxClients),
                _lightingSubsystem(LightingSubsystem),
                _htdocsDir(htdocsDir),
                _dataTransmissionTimeout(dataTransmissionTimeout)
        {}
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
				requestFetched = reader.read(socket(), isl::Timestamp::limit(_service._dataTransmissionTimeout), &bytesReadFromDevice);
				if (requestFetched) {
					isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "HTTP-request has been fetched, bytesReadFromDevice = ") << bytesReadFromDevice);
				} else {
					isl::Log::warning().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "HTTP-request have NOT been fetched, bytesReadFromDevice = ") << bytesReadFromDevice);
				}
			} catch (std::exception& e) {
				isl::Log::error().log(isl::ExceptionLogMessage(SOURCE_LOCATION_ARGS, e));
				return;
			} catch (...) {
				isl::Log::error().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Unknown error occured"));
				return;
			}
                        // Generating and sending response if uncomplete request has been received
			if (!requestFetched) {
				if (parser.isBad()) {
                                        sendError(400, parser.error()->message(), "Bad request");
				} else {
                                        sendError(408, "Request timeout expired");
				}
                                return;
			}
                        // Inspecting URI
                        std::string path = (reader.path() == "/") ? INDEX_PATH : reader.path();
                        if (path.empty() || path[0] != '/' || path.find("..") != std::string::npos) {
                                std::ostringstream oss;
                                oss << "Invalid URI path &quot;" << path << "&quot;";
                                sendError(400, oss.str(), "Bad request");
                                return;
                        }
                        // Check for lighting subsystem call
                        if (reader.path() == "/lighting") {
                                handleLighting(reader);
                                return;
                        }
                        // Trying to send a file
                        sendFile(path);
                }

                void sendError(int statusCode, const std::string& errMsg, const std::string& errDesc = std::string())
                {
			std::ostringstream oss;
			oss << "<html><head><title>Greenhouse server WEB-console</title></head><body><p>";
                        if (!errDesc.empty()) {
                                oss << errDesc << ": ";
                        }
                        oss << errMsg << "</p></body></html>";
                        isl::HttpResponseStreamWriter responseWriter(statusCode);
                        responseWriter.setHeaderField("Content-Type", "text/html; charset=utf-8");
                        responseWriter.setHeaderField("Connection", "close");
                        responseWriter.writeOnce(socket(), oss.str(), isl::Timestamp::limit(_service._dataTransmissionTimeout));
                }

                void sendFile(const std::string& uri)
                {
                        std::string filename = _service._htdocsDir;
                        filename += uri;
                        std::ifstream fs(filename.c_str());
                        if (!fs.good()) {
                                std::ostringstream oss;
                                oss << "Resourse &quot;" << uri << "&quot; not found on server";
                                sendError(404, oss.str(), "Not found");
                                return;
                        }
                        const char * mt = mimeType(filename);
                        if (!mt) {
                                std::ostringstream oss;
                                oss << "No MIME-type for &quot;" << uri << "&quot; resourse";
                                sendError(500, oss.str(), "Internal server error");
                                return;
                        }
                        std::string str((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
                        isl::HttpResponseStreamWriter responseWriter;
                        responseWriter.setHeaderField("Content-Type", mt);
                        responseWriter.setHeaderField("Connection", "close");
                        responseWriter.writeOnce(socket(), str, isl::Timestamp::limit(_service._dataTransmissionTimeout));
                }

                void handleLighting(const isl::HttpRequestReader& requestReader)
                {
                        std::string mode = isl::Http::paramValue(requestReader.get(), "mode");
                        std::ostringstream oss;
                        if (mode == "set_manual_lamp_supply") {
                                // Request to set manual lamp supply level
                                std::string s = isl::Http::paramValue(requestReader.get(), "level");
                                std::istringstream ss(s);
                                int level;
                                ss >> level;
                                if (ss.fail()) {
                                        oss << "{\"status\":\"error\",\"message\":\"Invalid lamp supply level value\"}";
                                } else {
                                        if (_service._lightingSubsystem.setManualLampSupplyLevel(level)) {
                                                oss << "{\"status\":\"ok\"}";
                                        } else {
                                                oss << "{\"status\":\"error\",\"message\":\"Setting manual lamp supply level value error\"}";
                                        }
                                }
                        } else if (mode == "reset_manual_lamp_supply") {
                                // Request to reset manual lamp supply level
                                if (_service._lightingSubsystem.resetManualLampSupplyLevel()) {
                                        oss << "{\"status\":\"ok\"}";
                                } else {
                                        oss << "{\"status\":\"error\",\"message\":\"Resetting manual lamp supply level error\"}";
                                }
                        } else {
                                // Request status from the subsystem and return a result
                                std::auto_ptr<LightingSubsystem::Status> statusAutoPtr(_service._lightingSubsystem.requestStatus());
                                if (statusAutoPtr.get()) {
                                        oss << "{\"status\":\"ok\",\"statusRegisterValue\":" << statusAutoPtr->statusRegisterValue <<
                                                ",\"lightSensorRegisterValue\":" << statusAutoPtr->lightSensorRegisterValue <<
                                                ",\"lampSupplyLevelRegisterValue\":" << statusAutoPtr->lampSupplyLevelRegisterValue <<
                                                ",\"currentLightIntensity\":" << statusAutoPtr->currentLightIntensity <<
                                                ",\"desiredLightIntensity\":" << statusAutoPtr->desiredLightIntensity <<
                                                ",\"isManualDesiredLightIntensity\":" << std::boolalpha << statusAutoPtr->isManualDesiredLightIntensity <<
                                                ",\"isManualLampSupplyLevel\":" << std::boolalpha << statusAutoPtr->isManualLampSupplyLevel << "}";
                                } else {
                                        oss << "{\"status\":\"error\",\"message\":\"Fetching status from the lighting subsystem error\"}";
                                }
                        }
                        isl::HttpResponseStreamWriter responseWriter;
                        responseWriter.setHeaderField("Content-Type", "application/json");
                        responseWriter.setHeaderField("Connection", "close");
                        responseWriter.writeOnce(socket(), oss.str(), isl::Timestamp::limit(_service._dataTransmissionTimeout));
                }

                HttpService& _service;
        };
        //! Task creation factory method definition
        virtual AbstractTask * createTask(isl::TcpSocket& socket)
        {
                return new HttpTask(*this, socket);
        }

        static const char * mimeType(const std::string& filename)
        {
                static const char * jsMimeType = "application/javascript";
                static const char * cssMimeType = "text/css";
                static const char * htmlMimeType = "text/html";
                static const char * jpegMimeType = "image/jpeg";
                static const char * gifMimeType = "image/gif";
                static const char * pngMimeType = "image/png";

                std::string ext = fileExtension(filename);
                if (ext == "js") {
                        return jsMimeType;
                } else if (ext == "css") {
                        return cssMimeType;
                } else if (ext == "html") {
                        return htmlMimeType;
                } else if (ext == "jpg" || ext == "jpeg") {
                        return jpegMimeType;
                } else if (ext == "gif") {
                        return gifMimeType;
                } else if (ext == "png") {
                        return pngMimeType;
                } else {
                        return 0;
                }
        }

        static std::string fileExtension(const std::string& filename)
        {
                size_t dotPos = filename.rfind('.');
                if (dotPos == std::string::npos) {
                        return std::string();
                }
                size_t slashPos = filename.rfind('/');
                if (slashPos != std::string::npos && slashPos > dotPos) {
                        return std::string();
                }
                return filename.substr(dotPos + 1);
        }

        LightingSubsystem& _lightingSubsystem;
        std::string _htdocsDir;
        isl::Timeout _dataTransmissionTimeout;
};

#endif
