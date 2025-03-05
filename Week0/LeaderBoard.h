#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;
class ULeaderboard {
public:
    std::vector<int> Scores;  // 단순히 점수만 저장
    const int MaxScores = 5;  // 저장할 최대 점수 개수
    const std::string FileName = "leaderboard.txt";  // 파일 이름

    // 리더보드 로드
    void LoadScores() {
        std::ifstream file(FileName);
        if (file.is_open()) {
            int score;
            while (file >> score) {
                Scores.push_back(score);
            }
            file.close();
        }
        // 점수 리스트를 내림차순으로 정렬
        std::sort(Scores.begin(), Scores.end(), std::greater<int>());
    }

    // 리더보드 저장
    void SaveScores() {
        std::ofstream file(FileName);
        if (file.is_open()) {
            for (int i = 0; i < min(MaxScores, (int)Scores.size()); ++i) {
                file << Scores[i] << "\n";
            }
            file.close();
        }
    }

    // 새로운 점수를 리더보드에 추가
    void AddScore(int newScore) {
        Scores.push_back(newScore);
        std::sort(Scores.begin(), Scores.end(), std::greater<int>());  // 내림차순 정렬
        if (Scores.size() > MaxScores) {
            Scores.resize(MaxScores);  // 상위 5개만 유지
        }
    }

    // 리더보드 출력
    void PrintLeaderboard() const {
        std::cout << "Leaderboard:\n";
        for (int i = 0; i < Scores.size(); ++i) {
            std::cout << i + 1 << ". " << Scores[i] << "\n";
        }
    }
};
