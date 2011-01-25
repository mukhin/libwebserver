// Hello World Demo.
//
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include <base/basicmacros.h>
#include <base/c_format.h>
#include <base/exception.h>
#include <base/rng.h>
#include <clock/clock.h>
#include <clock/hirestimer.h>
#include "scheduler.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cerrno> // for ::strerror and errno
#include <csignal> // for ::signal and SIG***


extern "C" {
#include <grp.h>
#include <pwd.h>

#ifdef HAVE_PRCTL
# include <sys/prctl.h>
#endif

#include <sys/types.h>
#include <unistd.h>
}

#ifdef ENABLE_PROFILING
# include <Saturn.h>
#endif


static bool GOT_STOP_SIGNAL = false;
static bool PAST_SCHEDULER_START = false;


void Exit(const int code, const std::string& message) {
  std::cerr << message << std::endl;
  if (code != 0) {
    ::exit(code);
  }
}


void signal_handler(int sig) {
  switch (sig) {
    case SIGHUP:
      std::cerr << "Got reconfiguration request, doing nothing yet..." << std::endl;
      break;
    case SIGTERM:
      std::cerr << "Got SIGTERM, shutting down..." << std::endl;
      GOT_STOP_SIGNAL = true;
      if (PAST_SCHEDULER_START) {
        while (!demo::Scheduler::Self()->IsActive()) {
          clocks::Clock::Sleep(1);
        }
        demo::Scheduler::Self()->Stop();
      }
      break;
    case SIGINT:
      std::cerr << "Got SIGINT, shutting down..." << std::endl;
      GOT_STOP_SIGNAL = true;
      if (PAST_SCHEDULER_START) {
        while (!demo::Scheduler::Self()->IsActive()) {
          clocks::Clock::Sleep(1);
        }
        demo::Scheduler::Self()->Stop();
      }
      break;
    default:
      // Shouldn't be here.
      ASSERT(false);
      break;
  }
}


// Will run simple "Hello World" non-demonized webserver.
int main() {
  ::signal(SIGHUP, signal_handler);
  ::signal(SIGTERM, signal_handler);
  ::signal(SIGINT, signal_handler);
  ::signal(SIGPIPE, SIG_IGN);


  if (GOT_STOP_SIGNAL) {
    goto exitpoint;
  }

  try {
    PAST_SCHEDULER_START = true;
    demo::Scheduler::Init();
    demo::Scheduler::Self()->Setup();
    demo::Scheduler::Self()->Start();
    demo::Scheduler::Self()->Join();
    demo::Scheduler::Destroy();
  }
  catch (const IOException& e) {
    Exit(1, e.what());
  }
  catch (const std::runtime_error& e) {
    Exit(1, c_format("Runtime error: %s.", e.what()));
  }
  catch (const std::exception& e) {
    Exit(1, c_format("Exception: %s.", e.what()));
  }
  catch (...) {
    Exit(1, "Unhandled exception. Performing planejump into another dimension...");
  }

exitpoint:
  // Prints message and removes pid only.
  Exit(0, "Shutdown complete.");

  return 0;
}
