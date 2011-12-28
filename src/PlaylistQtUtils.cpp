/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#include "PlaylistQtUtils.h"

#if defined(HAS_QT)

#include <QDir>
#include <QFileInfo>

using namespace pu;

/*
std::string QtFileTraits::absDir(const char* sourcePath) const {
  return QFileInfo(sourcePath).absoluteDir().absolutePath().toStdString();
}

std::string QtFileTraits::absName(const char* sourcePath) const {
  return QFileInfo(sourcePath).fileName().toStdString();
}

std::string QtFileTraits::absPath(const char* sourcePath) const {
  return QFileInfo(sourcePath).absoluteFilePath().toStdString();
}
*/

///////////////////////////////////////////////////////////////////////////

bool QtFileTraits::copy(const char* sourcePath, const char* destPath) const {
  return QFile(sourcePath).copy(destPath);
}

bool QtFileTraits::rename(const char* sourcePath, const char* destName) const {
  return QFile(sourcePath).rename(destName);
}

bool QtFileTraits::remove(const char* sourcePath) const {
  return QFile(sourcePath).remove();
}

bool QtFileTraits::move(const char* sourcePath, const char* destPath) const {
  return copy(sourcePath, destPath) && remove(sourcePath);
}

#endif