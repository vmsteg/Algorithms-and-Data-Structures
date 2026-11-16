# Отчёт по лабораторной работе №8 (Вариант 5, продвинутый уровень)

**Тема:** Двоичный поиск. Хеш-таблицы
**Дисциплина:** «Алгоритмы и структуры данных»

**Выполнила:**
Студентка группы ПИЖ-б-о-25-1 (2), направление подготовки: 09.03.04 «Программная инженерия»
Осипенко Юлия Юрьевна

**Проверила:**
Ассистент кафедры программной инженерии
Белик Ангелина Игоревна

Ставрополь 2026

---

## Цель работы

1. Реализовать алгоритм двоичного (бинарного) поиска в отсортированном массиве.
2. Освоить итеративную и рекурсивную реализации двоичного поиска.
3. Изучить понятие хеш-таблицы, хеш-функции и методы разрешения коллизий.
4. Реализовать хеш-таблицу с методом цепочек и открытой адресацией.
5. Сравнить производительность двоичного поиска и хеш-таблицы.

---

## Теоретическая часть

**Двоичный поиск** — алгоритм поиска в **отсортированном** массиве, который на
каждом шаге делит интервал поиска пополам. Сложность — **O(log n)**.

**Хеш-таблица** — структура данных «ключ → значение». Хеш-функция отображает
ключ в индекс бакета. При коллизии (разные ключи → один индекс) используются
методы разрешения: **цепочки** (связные списки) или **открытая адресация**
(линейное/квадратичное пробирование, двойное хеширование).

| Структура | Поиск (средн.) | Поиск (худш.) | Память |
| :--- | :--- | :--- | :--- |
| Двоичный поиск | O(log n) | O(log n) | O(1) |
| Хеш-таблица (цепочки) | O(1 + α) | O(n) | O(n + m) |

где **α = count / buckets** — коэффициент загрузки.

---

## Задание (Вариант 5, продвинутый уровень)

**Часть 2. Хеш-таблица:**
- метод разрешения коллизий: **метод цепочек**;
- тип ключа: **string**;
- тип значения: **int**.

**Часть 3. Сравнительный анализ (для всех вариантов):**
1. Реализовать класс хеш-таблицы согласно варианту.
2. Реализовать двоичный поиск (итеративно).
3. Сравнить производительность операций поиска для N = 1000, 10000, 100000:
   - двоичный поиск в отсортированном массиве;
   - поиск в хеш-таблице.
4. Построить таблицу/график зависимости времени от N.

---

## Листинг программы

```cpp
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include <random>
using namespace std;
using namespace chrono;

// ===================== Двоичный поиск (итеративный) =====================
int binarySearch(const vector<string>& a, const string& target) {
    int left = 0, right = (int)a.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (a[mid] == target) return mid;
        if (a[mid] < target)  left  = mid + 1;
        else                  right = mid - 1;
    }
    return -1;
}

// ===================== Хеш-таблица: цепочки, string → int =====================
class HashTable {
    struct Node {
        string key;
        int value;
        Node* next;
        Node(const string& k, int v) : key(k), value(v), next(nullptr) {}
    };

    vector<Node*> table;
    int size;

    int hash(const string& key) const {
        unsigned long long h = 0;
        for (char c : key) h = (h * 31 + (unsigned char)c) % size;
        return (int)h;
    }

public:
    HashTable(int s) : size(s), table(s, nullptr) {}

    ~HashTable() {
        for (int i = 0; i < size; ++i) {
            Node* cur = table[i];
            while (cur) { Node* t = cur; cur = cur->next; delete t; }
        }
    }

    void insert(const string& key, int value) {
        int i = hash(key);
        for (Node* cur = table[i]; cur; cur = cur->next)
            if (cur->key == key) { cur->value = value; return; }
        Node* n = new Node(key, value);
        n->next = table[i];
        table[i] = n;
    }

    bool find(const string& key, int& value) const {
        int i = hash(key);
        for (Node* cur = table[i]; cur; cur = cur->next)
            if (cur->key == key) { value = cur->value; return true; }
        return false;
    }
};

// ===================== Замер одного размера N =====================
void bench(int N) {
    vector<string> keys;
    for (int i = 0; i < N; ++i) keys.push_back("key_" + to_string(i));

    HashTable ht(N * 1.3 + 1);
    for (int i = 0; i < N; ++i) ht.insert(keys[i], i);

    vector<string> sorted = keys;
    sort(sorted.begin(), sorted.end());

    vector<string> queries;
    mt19937 rng(42);
    uniform_int_distribution<int> d(0, N - 1);
    for (int i = 0; i < 10000; ++i) queries.push_back(keys[d(rng)]);

    auto t1 = high_resolution_clock::now();
    for (const auto& q : queries) binarySearch(sorted, q);
    auto t2 = high_resolution_clock::now();

    int v;
    auto t3 = high_resolution_clock::now();
    for (const auto& q : queries) ht.find(q, v);
    auto t4 = high_resolution_clock::now();

    double binUs  = duration_cast<nanoseconds>(t2 - t1).count() / 1000.0;
    double hashUs = duration_cast<nanoseconds>(t4 - t3).count() / 1000.0;

    cout << "N = " << N
         << "  | binary: " << binUs  << " мкс"
         << "  | hash: "   << hashUs << " мкс"
         << "  | x" << (binUs / hashUs) << "\n";
}

int main() {
    vector<string> a = {"apple", "banana", "cherry", "date"};
    sort(a.begin(), a.end());
    cout << "binarySearch(cherry) -> " << binarySearch(a, "cherry") << "\n";

    HashTable ht(11);
    ht.insert("apple", 1);
    ht.insert("banana", 2);
    int v;
    if (ht.find("banana", v)) cout << "hash: banana -> " << v << "\n";
    cout << "\n=== Сравнение (10000 запросов) ===\n";

    for (int N : {1000, 10000, 100000}) bench(N);
    return 0;
}