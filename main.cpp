#include <fstream>
#include <iostream>
#include <regex>

#include "../include/file.hpp"
#include "../include/ofApp.hpp"
#include "ofMain.h"

namespace fs = std::filesystem;

const std::regex r_pos =
    std::regex(R"(\((\d+),(\d+),?([XY]{1}_[A-Z]{4,5})?\),)");

int main(int argc, char* argv[]) {
  // デフォルトのファイルパスを指定
  const std::string MapFile = "assets/VER-1.map";
  const std::string TxtFolder = "assets/inst_1/";

  //fs::path TxtFolder = "/home/leus/mapf-visualizer/assets/inst_1";
  std::vector<fs::path> Files;
  std::vector<int> initialTimes;  // 各ファイルの初期時刻を格納

  for (const auto& entry : fs::directory_iterator(TxtFolder)) {
    if (entry.is_regular_file()) {
        Files.push_back(entry.path());
    }
  }

  std::sort(Files.begin(), Files.end());

  for (size_t i = 0; i < Files.size(); i++) {
    std::cout << i << ": " << Files[i].filename() << std::endl;
  
    // コマンドライン引数を省略した場合、デフォルトのパスを使用
    const std::string mapFilePath = (argc > 1) ? argv[1] : MapFile;
    const std::string txtFolderPath = (argc > 2) ? argv[2] : Files[i];

    // Initialize FileHandler
    FileHandler fileHandler(mapFilePath, txtFolderPath);

    // Load and modify the map
    Graph G = fileHandler.loadMap();
    

    // Combine all .txt files into one solution
    Solution combinedSolution;
    std::vector<Config> additionalGoals;
    int initialTime = 0;  // 初期時刻を保持する変数

    for (const auto& txtFile : Files) {  // Files全体をループ
      std::ifstream solutionFile(txtFile.string());
      if (!solutionFile.is_open()) {
        std::cerr << "Failed to open solution file: " << txtFile << std::endl;
        continue;
      }

      std::string line;
      std::smatch m;
      Config lastConfig;

      if (std::getline(solutionFile, line)) {  // 1行目を取得
        int fileInitialTime = std::stoi(line);  // このファイルの初期時刻を取得
        initialTimes.push_back(fileInitialTime);  // 初期時刻を保存
      }

      while (std::getline(solutionFile, line)) {
        if (line.find(":(") == std::string::npos) continue;

        auto iter = line.cbegin();
        Config c;
        while (iter < line.cend()) {
          auto search_end = std::min(iter + 128, line.cend());
          if (std::regex_search(iter, search_end, m, r_pos)) {
            auto x = std::stoi(m[1].str());
            auto y = std::stoi(m[2].str());
            Orientation o = Orientation::NONE;
            if (m[3].matched) {
              o = Orientation::from_string(m[3].str());
            }
            c.push_back(Pose(G.U[G.width * y + x], o));
            iter += m[0].length();
          } else {
            break;
          }
        }
        combinedSolution.push_back(c);
        lastConfig = c;
      }
      additionalGoals.push_back(lastConfig);
      solutionFile.close();
    }

  // Visualize the combined solution
  ofSetupOpenGL(100, 100, OF_WINDOW);
  ofRunApp(new ofApp(&G, &combinedSolution, additionalGoals, initialTime,
                     (argc > 3 && std::string(argv[3]) == "--capture-only")));

  }

    return 0;

}