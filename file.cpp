#include "../include/file.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>

namespace fs = std::filesystem;

FileHandler::FileHandler(const std::string& mapFilePath, const std::string& txtFolderPath)
    : mapFilePath(mapFilePath), txtFolderPath(txtFolderPath) {}

Graph FileHandler::loadMap() {
  return Graph(const_cast<char*>(mapFilePath.c_str()));
}

std::vector<std::string> FileHandler::loadTxtFiles() {
  std::vector<std::string> txtFiles;
  for (const auto& entry : fs::directory_iterator(txtFolderPath)) {
    if (entry.path().extension() == ".txt") {
      txtFiles.push_back(entry.path().string());
    }
  }
  // アルファベット順にソート
  std::sort(txtFiles.begin(), txtFiles.end());
  return txtFiles;
}