#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <windows.h>
#include <yy981/path.h>
#include <yy981/sha256.h>

namespace fs = std::filesystem;

inline void SetConsoleTextColorRed() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
}

inline void ResetConsoleTextColor() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            std::cerr << "使用方法: " << argv[0] << " <フォルダパス1> [フォルダパス2] ...\n";
            return 1;
        }

        std::unordered_map<std::string, std::vector<fs::path>> hashToFileMap;

        for (int i = 1; i < argc; ++i) {
            fs::path folderPath = argv[i];
            if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
                std::cerr << "無効なフォルダパス: " << folderPath << "\n";
                continue;
            }

            for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
                if (!entry.is_regular_file()) continue;

                try {
                    std::string hash = calculateSHA256(entry.path());
                    if (hash != "ERROR") hashToFileMap[hash].push_back(entry.path());
                } catch (const std::exception& e) {
                    std::cerr << "エラー: " << e.what() << "\n";
                }
            }
        }

        // 重複ファイルを出力
        for (const auto& [hash, files] : hashToFileMap) {
            if (files.size() > 1) {
                SetConsoleTextColorRed();  // 赤色に設定
                std::cout << hash << ":\n";
                ResetConsoleTextColor();  // 色リセット
                for (const auto& file : files) {
                    std::cout << "  " << file.string() << "\n";
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "致命的エラー: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
