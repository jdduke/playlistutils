#include <QApplication>

#include "PlaylistWindow.h"

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

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  qInstallMsgHandler(myMessageOutput);
  QString fileName;

  if (argc >= 2)
    fileName = argv[1];
  else
    fileName = ".";

  PlaylistWindow window(fileName);
  window.show();

  return app.exec();
}
