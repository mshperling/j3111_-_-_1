#include <iostream> 
#include <unordered_map> // хеш-таблицы. 
#include <vector> // динамические массивы. 
#include <string> // для работы со строками. .
using namespace std;

int main() {
    int n;
    cout << "размер массива: ";
    cin >> n;
    // ввод размера массива O(1). для хранения одного целого числа нужно 4 байта.

    vector<string> arr(n); // создаем вектор строк.
    // требует O(n) времени, так как нужно выделить память для n элементов. для хранения n строк понадобится 24 * n байт.

    cout << "сам массив: ";
    for (int i = 0; i < n; i++) {
        cin >> arr[i]; // ввод строк в массив.
        // ввод строки занимает O(m) времени, где m — длина строки. для всех строк это O(n * m).
        // память: каждая строка занимает m байт, суммарная память — n * m, где m — средняя длина строки.
    }

    unordered_map<string, int> countMap; // хеш-таблица для подсчета вхождений строк.
    // инициализация хеш-таблицы занимает O(1) времени и n + m памяти, где n — количество уникальных ключей, m — размер хеш-таблицы.

    for (const string& str : arr) {
        countMap[str]++; // увеличиваем счетчик для каждой строки.
        // доступ и обновление в unordered_map занимает O(1) в среднем. Для n строк сложность — O(n).
        // Память: для хранения ключа (строки) и значения (счетчика) в хеш-таблице потребуется n * m байт.
    }

    for (int i = 0; i < n; i++) {
        arr[i] = to_string(countMap[arr[i]]); // заменяем строку в массиве на её счетчик.
        // доступ к элементу countMap[arr[i]] за O(1). преобразование целого числа в строку — O(k), где k — количество цифр в числе.
        // весь цикл работает за O(n * k), где n — количество строк, k — средняя длина числа (обычно O(log(countMap[arr[i]]))).
    }

    cout << "итог: ";
    for (const string& str : arr) { // вывод строк без копирования.
        cout << str << " "; // вывод каждой строки занимает O(m), для всех строк — O(n * m).
    }

    return 0;
}
