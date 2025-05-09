#pragma once

#include <string>
#include <vector>
#include "graph.hpp"

class FileHandler {
public:
  FileHandler(const std::string& mapFilePath, const std::string& txtFolderPath);

  // Load the map
  Graph loadMap();

  // Load all .txt files from the folder
  std::vector<std::string> loadTxtFiles();

private:
  std::string mapFilePath;
  std::string txtFolderPath;
};