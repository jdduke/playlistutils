/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#include <QApplication>

#include <PlaylistUtilities.h>

#include "playlist_window.h"

#ifdef PU_WINDOWS

#include <Windows.h>

static void myMessageOutput(QtMsgType type, const char* msg) {
  static const size_t size = 1024;
  char buf[size];
  switch (type) {
  case QtDebugMsg:
    sprintf_s(buf, size, "Debug:    %s\n", msg);
    OutputDebugString(buf);
    break;
  case QtWarningMsg:
    sprintf_s(buf, size, "Warning:  %s\n", msg);
    OutputDebugString(buf);
    break;
  case QtCriticalMsg:
    sprintf_s(buf, size, "Critical: %s\n", msg);
    OutputDebugString(buf);
    break;
  case QtFatalMsg:
    sprintf_s(buf, size, "Fatal:    %s\n", msg);
    OutputDebugString(buf);
    abort();
  };
}

#else

static void myMessageOutput(QtMsgType type, const char* msg) {
  switch (type) {
  case QtDebugMsg:
    fprintf(stderr, "Debug: %s\n", msg);
    break;
  case QtWarningMsg:
    fprintf(stderr, "Warning: %s\n", msg);
    break;
  case QtCriticalMsg:
    fprintf(stderr, "Critical: %s\n", msg);
    break;
  case QtFatalMsg:
    fprintf(stderr, "Fatal: %s\n", msg);
    abort();
  };
}

#endif

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  qInstallMsgHandler(myMessageOutput);
  QString fileName;

  if (argc >= 2)
    fileName = argv[1];
  else
    fileName = ".";

  PlaylistWindow window;//(fileName);
  window.show();

  return app.exec();
}
