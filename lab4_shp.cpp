#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <cstdlib>
#include <limits>

using namespace std;

// Функция для расчета суммы отклонений от среднего значения в кластере
double calculateClusterCost(const vector<double>& cluster) {
    double mean = 0;
    for (double val : cluster) {
        mean += val;
    }
    mean /= cluster.size();

    double cost = 0;
    for (double val : cluster) {
        cost += abs(val - mean);
    }
    return cost;
}

// Рекурсивная функция для поиска оптимального разбиения
void findOptimalPartition(const vector<double>& arr, int index, int k, vector<vector<double>>& currentPartition, double currentCost, double& minCost, vector<vector<double>>& bestPartition) {
    if (index == arr.size() && currentPartition.size() == k) {
        if (currentCost < minCost) {
            minCost = currentCost;
            bestPartition = currentPartition;
        }
        return;
    }

    if (currentPartition.size() > k || index >= arr.size()) {
        return;
    }

    for (int i = 0; i < currentPartition.size(); ++i) {
        currentPartition[i].push_back(arr[index]);
        double addedCost = calculateClusterCost(currentPartition[i]);
        findOptimalPartition(arr, index + 1, k, currentPartition, currentCost + addedCost, minCost, bestPartition);
        currentPartition[i].pop_back();
    }

    if (currentPartition.size() < k) {
        currentPartition.push_back({arr[index]});
        findOptimalPartition(arr, index + 1, k, currentPartition, currentCost, minCost, bestPartition);
        currentPartition.pop_back();
    }
}
// Время: в худшем случае O(k^n) из-за рекурсии
// Память: в худшем случае O(n*k)

// Функция для вывода массива
void printArray(const vector<double>& arr) {
    cout << "[";
    for (size_t i = 0; i < arr.size(); ++i) {
        cout << arr[i];
        if (i != arr.size() - 1) {
            cout << ", ";
        }
    }
    cout << "]" << endl;
}

// Функция для вывода кластеров
void printClusters(const vector<vector<double>>& clusters) {
    for (int i = 0; i < clusters.size(); ++i) {
        cout << "[";
        for (int j = 0; j < clusters[i].size(); ++j) {
            cout << clusters[i][j];
            if (j != clusters[i].size() - 1) {
                cout << ", ";
            }
        }
        cout << "]";
        if (i != clusters.size() - 1) {
            cout << ", ";
        }
    }
    cout << endl;
}

// Функция для тестирования на массивах от 8 до 25
void runTests(int k) {
    for (int size = 10; size <= 25; ++size) {
        vector<double> testArray(size);
        for (int i = 0; i <= 25; ++i) {
            testArray[i] = rand() % 100 + 1; // Генерация случайных чисел от 1 до 100
        }

        cout << "Массив размером " << size << ": ";
        printArray(testArray);

        auto startTime = chrono::high_resolution_clock::now();

        vector<vector<double>> currentPartition;
        vector<vector<double>> bestPartition;
        double minCost = numeric_limits<double>::max();

        findOptimalPartition(testArray, 0, k, currentPartition, 0, minCost, bestPartition);

        auto endTime = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = endTime - startTime;

        cout << "Минимальная ошибка: " << fixed << setprecision(2) << minCost << endl;
        cout << "Время выполнения: " << elapsed.count() << " секунд" << endl;
        cout << "Разбиение на кластеры: ";
        printClusters(bestPartition);
        cout << endl;
    }
}

int main() {
    int k = 8; // Количество кластеров
    runTests(k);

    return 0;
}
