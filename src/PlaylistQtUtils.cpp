#include "PlaylistQtUtils.h"

#include <QDir>
#include <QFileInfo>

using namespace pu;

std::string QtFileTraits::absDir(const char* sourcePath) const {
  return QFileInfo(sourcePath).absoluteDir().absolutePath().toStdString();
}

std::string QtFileTraits::absName(const char* sourcePath) const {
  return QFileInfo(sourcePath).fileName().toStdString();
}

std::string QtFileTraits::absPath(const char* sourcePath) const {
  return QFileInfo(sourcePath).absoluteFilePath().toStdString();
}

///////////////////////////////////////////////////////////////////////////

bool QtFileTraits::copy(const char* sourcePath, const char* destPath) const {
  return QFile::copy(sourcePath, destPath);
}

bool QtFileTraits::rename(const char* sourcePath, const char* destName) const {
  return QFile::rename(sourcePath, destName);
}

bool QtFileTraits::remove(const char* sourcePath) const {
  return QFile::remove(sourcePath);
}

bool QtFileTraits::move(const char* sourcePath, const char* destPath) const {
  return QFile::copy(sourcePath, destPath) && remove(sourcePath);
}
