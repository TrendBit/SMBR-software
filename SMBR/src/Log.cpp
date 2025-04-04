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



namespace SMBR {

		

		void threadDebug() {
			int tid = 0;

			#ifndef _WIN32
						 tid = (pid_t) syscall(SYS_gettid);
			#endif

			Poco::Thread * thread = Poco::Thread::current();
			if (!thread){
				LINFO("THRDBG") << "Thread " << "MAIN" << " has Thread ID " << Poco::Thread::currentTid() << " and PID " << tid << LE;
				//std::cout <<         "Thread " << "MAIN" << " has Thread ID " << Poco::Thread::currentTid() << " and PID " << tid << std::endl;
			} else {
				LINFO("THRDBG") << "Thread " << thread->name() << " has Thread ID " << thread->tid() << " and PID " << tid << LE
				//std::cout <<         "Thread " << thread->name() << " has Thread ID " << thread->tid() << " and PID " << tid << std::endl;
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
