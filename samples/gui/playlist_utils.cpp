/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#include "playlist_utils.h"

#include <QDir>
#include <QFileInfo>

using namespace pu;

bool QtFileHandler::copy(const char* sourcePath, const char* destPath) const {
  return QFile(sourcePath).copy(destPath);
}

bool QtFileHandler::rename(const char* sourcePath, const char* destName) const {
  return QFile(sourcePath).rename(destName);
}

bool QtFileHandler::move(const char* sourcePath, const char* destPath) const {
  return copy(sourcePath, destPath) && remove(sourcePath);
}

bool QtFileHandler::remove(const char* sourcePath) const {
  return QFile(sourcePath).remove();
}

bool QtFileHandler::exists(const char* sourcePath) const {
  return QFile::exists(sourcePath);
}
