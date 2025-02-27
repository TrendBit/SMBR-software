#include "SMBR/Log.hpp"
#include <Poco/Thread.h>
#include <Poco/SimpleFileChannel.h>
#include "Poco/AutoPtr.h"
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>
#include <Poco/SyslogChannel.h>
#include <iostream>
#include <set>
#include <Poco/Glob.h>

#include <sstream>

#include <Poco/Exception.h>
#include <Poco/StringTokenizer.h>
#include <Poco/String.h>

#include <Poco/File.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeFormat.h>
#include "Poco/Channel.h"

#include <Poco/NumberParser.h>
#include "Poco/NumberFormatter.h"
#include <string>

#ifndef _WIN32
#include <unistd.h>
#include <sys/syscall.h>

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

#endif

namespace SMBR {

	std::string logBasename(const std::string& fullname){
#ifdef _WIN32
		size_t last_delim = fullname.rfind('\\');
#else
		size_t last_delim = fullname.rfind('/');
#endif
		if (last_delim == std::string::npos) return fullname;
		else return fullname.substr(last_delim+1);
	}

}

#include <Poco/Channel.h>
#include <Poco/Message.h>
#include <Poco/SplitterChannel.h>
#include <iostream>

class ColorConsoleChannel : public Poco::Channel {
	public:
		typedef Poco::AutoPtr <ColorConsoleChannel> Ptr;

		ColorConsoleChannel() { }
		virtual ~ColorConsoleChannel() { }

		void log(const Poco::Message& msg){
			Poco::LocalDateTime t;
			std::string lctime = Poco::DateTimeFormatter::format(t, "%M:%S.%i");
#ifndef _WIN32


			std::string color = "\033[38;5;146m";
			//std::string utctime = Poco::DateTimeFormatter::format(t.utc(), "%Y-%m-%dT%H:%M:%S.%i");
			//if (msg.getPriority() >= Poco::Message::PRIO_WARNING){
				//color = "\033[38;5;149m";
				if (msg.getPriority() == Poco::Message::PRIO_TRACE){
					color = "\033[38;5;247m";
				}
				if (msg.getPriority() == Poco::Message::PRIO_INFORMATION){
					color = "\033[38;5;147m";
				}
				if (msg.getPriority() == Poco::Message::PRIO_NOTICE){
					color = "\033[38;5;74m";
				}
				if (msg.getPriority() == Poco::Message::PRIO_WARNING){
					color = "\033[38;5;148m";
				}
				if (msg.getPriority() == Poco::Message::PRIO_ERROR){
					color = "\033[38;5;196m";
				}
				if (msg.getPriority() <= Poco::Message::PRIO_CRITICAL){
					color = "\033[38;5;200m";
				}
			//}
			std::string colorEnd = "\033[39m";
			std::cout << color << lctime << ": " << msg.getText() << colorEnd << std::endl;
#else
			std::cout << lctime << ": " << msg.getText() << std::endl;
#endif

		}

};

namespace SMBR {

		LogParams::LogParams(){
			printToCout = true;
			deleteOld = true;
			rotation = "1024 K";

		}
		LogParams::LogParams(bool p, bool deleteOldLogs){
			printToCout = p;
			deleteOld = deleteOldLogs;
			rotation = "1024 K";
		}

		void threadDebug() {
			int tid = 0;

			#ifndef _WIN32
						 tid = (pid_t) syscall(SYS_gettid);
			#endif

			Poco::Thread * thread = Poco::Thread::current();
			if (!thread){
				LERROR("THRDBG") << "Thread " << "MAIN" << " has Thread ID " << Poco::Thread::currentTid() << " and PID " << tid << LE;
				std::cout <<         "Thread " << "MAIN" << " has Thread ID " << Poco::Thread::currentTid() << " and PID " << tid << std::endl;
			} else {
				LERROR("THRDBG") << "Thread " << thread->name() << " has Thread ID " << thread->tid() << " and PID " << tid << LE
				std::cout <<         "Thread " << thread->name() << " has Thread ID " << thread->tid() << " and PID " << tid << std::endl;
			}
		}

		void shortFileList(std::set<std::string> files, unsigned int targetListSize) {
			try {
				while (files.size() > targetListSize) {

					std::set<std::string>::iterator it = files.begin();
					Poco::File fToDelete(*it);
					++it;
					for (; it != files.end(); ++it) {

						Poco::File tmpFile(*it);
						if (tmpFile.exists() && tmpFile.created() < fToDelete.created()) {
							fToDelete = tmpFile;
						}
					}
					fToDelete.remove();
					files.erase(fToDelete.path());
				}
			} catch (Poco::Exception & e){
				std::cerr << "shortFileList exception: " << e.displayText() << std::endl;
			}
		}

		void separeLog(std::string logName, std::string fileName, int level, LogParams params){

				if (params.deleteOld){
					if (Poco::File(fileName).exists()){
						Poco::File(fileName).remove();
					}
					if (Poco::File(fileName + ".0").exists()){
						Poco::File(fileName + ".0").remove();
					}
				}
				Poco::AutoPtr<Poco::SimpleFileChannel> identChannel(new Poco::SimpleFileChannel);

				identChannel->setProperty("path", fileName);
				identChannel->setProperty("rotation", params.rotation);

				Poco::AutoPtr<Poco::PatternFormatter> identPF(new Poco::PatternFormatter);
				identPF->setProperty("pattern", "%q %Y-%m-%dT%H:%M:%S.%i %s<%T>: %t");
				Poco::AutoPtr<Poco::FormattingChannel> identFC(new Poco::FormattingChannel(identPF, identChannel));


				Poco::AutoPtr<Poco::SplitterChannel> pSplitter(new Poco::SplitterChannel());
				pSplitter->addChannel(identFC);
				if (params.printToCout) {
					Poco::AutoPtr<ColorConsoleChannel> pColor(new ColorConsoleChannel());
					pSplitter->addChannel(pColor);
				}

				Poco::Logger::get(logName).setChannel(pSplitter);
				Poco::Logger::get(logName).setLevel((Poco::Message::Priority)level);

				size_t len = logName.size();
				std::vector <std::string> names;
				Poco::Logger::get(logName).names(names);
				for (auto n : names){
					if(n.size() >= len && n.substr(0, len) == logName){
						Poco::Logger::get(n).setChannel(pSplitter);
						Poco::Logger::get(n).setLevel((Poco::Message::Priority)level);
					}
				}

				LNOTICE(logName) << "log init" << LE

		}

				std::string storableFName(const std::string& name) {
#ifdef _WIN32
					return Poco::replace(name, ":", "-");
#endif
					return name;
				}

				static void initLogsInternally(
					std::string logName,
					int level,
					std::string logDir,
					LogHistory * history = nullptr,
					Poco::Channel * channel = nullptr,
					LogParams params = LogParams()
				) {
					std::cout << "init log " << logName << " to level " << level << std::endl;
					std::string logFile =  logName + std::string (".log");
					std::string logFile0 = logName + std::string (".log.0");

					Poco::File f(logDir + logFile);
					Poco::File f0(logDir + logFile0);
					if (f.exists()) {
						if (history){
							std::set<std::string> logFiles;
							std::set<std::string> logFiles0;
							Poco::Glob::glob(history->path + logFile + "_*", logFiles);
							Poco::Glob::glob(history->path + logFile0 + "_*", logFiles0);
							shortFileList(logFiles, history->size);
							shortFileList(logFiles0, history->size);
							Poco::Timestamp now;
							try {
								f.moveTo(storableFName(history->path + logFile + "_" + Poco::DateTimeFormatter::format(now, Poco::DateTimeFormat::ISO8601_FORMAT)));
							} catch (...) {
								f.remove();
							}
							if (f0.exists()) {
								try {
									f0.moveTo(storableFName(history->path + logFile0 + "_" + Poco::DateTimeFormatter::format(now, Poco::DateTimeFormat::ISO8601_FORMAT)));
								} catch (...) {
									f0.remove();
								}
							}
						} else {
							f.remove();
							if (f0.exists()) {
								f0.remove();
							}
						}
					}

					Poco::AutoPtr<Poco::SimpleFileChannel> pCons(new Poco::SimpleFileChannel);

					pCons->setProperty("path", logDir + logFile);
					pCons->setProperty("rotation", params.rotation);

					Poco::AutoPtr<Poco::PatternFormatter> pPF(new Poco::PatternFormatter);
					pPF->setProperty("pattern", "%q %Y-%m-%dT%H:%M:%S.%i %s<%T>: %t");
					Poco::AutoPtr<Poco::FormattingChannel> pFC(new Poco::FormattingChannel(pPF, pCons));

					Poco::AutoPtr<Poco::SplitterChannel> pSplitter(new Poco::SplitterChannel());
					pSplitter->addChannel(pFC);
					if (params.printToCout) {
						Poco::AutoPtr<ColorConsoleChannel> pColor(new ColorConsoleChannel());
						pSplitter->addChannel(pColor);
					}

					if (channel != NULL){
						pSplitter->addChannel(channel);
					}


					Poco::Logger::root().setChannel(pSplitter);
					Poco::Logger::root().setLevel((Poco::Message::Priority)level);

					std::vector <std::string> names;
					Poco::Logger::root().names(names);
					for (auto n : names){
						Poco::Logger::get(n).setChannel(pSplitter);
						Poco::Logger::get(n).setLevel((Poco::Message::Priority)level);
					}

					threadDebug();

				}

				void initLogs(std::string logName, int level, std::string logDir, LogParams params){
					initLogsInternally(logName, level, logDir, nullptr, nullptr, params);
				}

				void initLogs(
					std::string logName,
					int level,
					std::string logDir,
					LogHistory history,
					Poco::Channel * channel,
					LogParams params
				) {
					initLogsInternally(logName, level, logDir, &history, channel, params);
				}

				void initSepareLogs(const std::set<std::string> & names, std::string dir, std::string prefix, LogParams params){
					std::vector<std::string> existing;
					Poco::Logger::names(existing);

					for (auto log : existing) {
						auto& logger = Poco::Logger::get(log);
						logger.setLevel(Poco::Logger::root().getLevel());
						logger.setChannel(Poco::Logger::root().getChannel());
					}
					for (std::set<std::string>::const_iterator slog = names.begin(); slog != names.end(); ++slog){
						std::string slogname = *slog;

						//try parse for @ for distinguish log level
						Poco::StringTokenizer stoken(slogname, "@");
						int level = 8;
						if (stoken.count() == 2 && Poco::NumberParser::tryParse(stoken[1], level)){
							slogname = stoken[0];
							separeLog(slogname, dir + prefix + slogname +".log", level, params);
						} else {
							separeLog(slogname, dir + prefix + slogname +".log", 8, params);
						}
					}
				}

				void dumpBacktrace(std::string name, std::string logname, int level){
#ifndef _WIN32

				   Poco::Logger & pl = Poco::Logger::get(logname);

				   if (pl.getLevel() >= level){

					   void *array[10];
					   size_t size;
					   char **strings;
					   size_t i;

					   size = backtrace (array, 10);
					   strings = backtrace_symbols (array, size);

					   Poco::LogStream str(pl);
					   str.priority((Poco::Message::Priority)level);
					   str << name << " - backtrace: " << size << " stack frames (in " << ::SMBR::logBasename(__FILE__) << ":" << __func__ << ":" << __LINE__ << ")"<< std::endl;

					   for (i = 0; i < size; i++){
						   str  << " > " << strings[i] << " (in " << ::SMBR::logBasename(__FILE__) << ":" << __func__ << ":" << __LINE__ << ")"<< std::endl;
					   }

					   free (strings);
				   }
#endif

				}


}
