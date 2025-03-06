#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;
class ULeaderboard {
public:
    std::vector<int> Scores;  // �ܼ��� ������ ����
    const int MaxScores = 5;  // ������ �ִ� ���� ����
    const std::string FileName = "leaderboard.txt";  // ���� �̸�

    // �������� �ε�
    void LoadScores() {
        std::ifstream file(FileName);
        if (file.is_open()) {
            int score;
            while (file >> score) {
                Scores.push_back(score);
            }
            file.close();
        }
        // ���� ����Ʈ�� ������������ ����
        std::sort(Scores.begin(), Scores.end(), std::greater<int>());
    }

    // �������� ����
    void SaveScores() {
        std::ofstream file(FileName);
        if (file.is_open()) {
            for (int i = 0; i < min(MaxScores, (int)Scores.size()); ++i) {
                file << Scores[i] << "\n";
            }
            file.close();
        }
    }

    // ���ο� ������ �������忡 �߰�
    void AddScore(int newScore) {
        Scores.push_back(newScore);
        std::sort(Scores.begin(), Scores.end(), std::greater<int>());  // �������� ����
        if (Scores.size() > MaxScores) {
            Scores.resize(MaxScores);  // ���� 5���� ����
        }
    }

    // �������� ���
    void PrintLeaderboard() const {
        std::cout << "Leaderboard:\n";
        for (int i = 0; i < Scores.size(); ++i) {
            std::cout << i + 1 << ". " << Scores[i] << "\n";
        }
    }
};
