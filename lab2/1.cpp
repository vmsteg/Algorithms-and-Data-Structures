#include <iostream>
#include <stdexcept>
#include <chrono>
#include <deque>
#include <vector>
using namespace std;
using namespace chrono;

template <typename T>
class Deque {
private:
    T* data;          // массив для элементов
    int capacity;     // текущая ёмкость
    int frontIndex;   
    int backIndex;    
    int count;        

    // O(n) — расширение массива в 2 раза
    void resize() {
        int newCapacity = capacity * 2;
        T* newData = new T[newCapacity];
        for (int i = 0; i < count; i++)
            newData[i] = data[(frontIndex + i) % capacity];   // копирование по порядку
        delete[] data;
        data = newData;
        capacity = newCapacity;
        frontIndex = 0;
        backIndex = count - 1;
    }

public:
    // O(1) — конструктор
    Deque(int initialCapacity = 8)
        : capacity(initialCapacity), frontIndex(0), backIndex(-1), count(0)
    {
        if (initialCapacity <= 0) throw invalid_argument("capacity > 0");
        data = new T[capacity];
    }

    ~Deque() { delete[] data; }

    // запрет копирования во избежание двойного delete
    Deque(const Deque&) = delete;
    Deque& operator=(const Deque&) = delete;

    // O(1) амортизированно
    void push_front(const T& value) {
        if (count == capacity) resize();
        frontIndex = (frontIndex - 1 + capacity) % capacity;   // сдвиг влево по кругу
        data[frontIndex] = value;
        count++;
    }

    // O(1) амортизированно
    void push_back(const T& value) {
        if (count == capacity) resize();
        backIndex = (backIndex + 1) % capacity;                // сдвиг вправо по кругу
        data[backIndex] = value;
        count++;
    }

    // O(1)
    T pop_front() {
        if (count == 0) throw out_of_range("Deque пуст");
        T value = data[frontIndex];                            // сохранение значения
        frontIndex = (frontIndex + 1) % capacity;              // сдвиг вправо
        count--;
        return value;
    }

    // O(1)
    T pop_back() {
        if (count == 0) throw out_of_range("Deque пуст");
        T value = data[backIndex];                             // сохранение значения
        backIndex = (backIndex - 1 + capacity) % capacity;     // сдвиг влево
        count--;
        return value;
    }

    // O(1) — вычисление позиции по формуле
    T& operator[](int index) {
        if (index < 0 || index >= count) throw out_of_range("Индекс вне диапазона");
        return data[(frontIndex + index) % capacity];
    }

    // O(1) — для константного объекта
    const T& operator[](int index) const {
        if (index < 0 || index >= count) throw out_of_range("Индекс вне диапазона");
        return data[(frontIndex + index) % capacity];
    }

    int size()  const { return count; }         // O(1)
    bool empty() const { return count == 0; }   // O(1)

    // O(n) — вывод всех элементов по очереди
    void print() const {
        cout << "[";
        for (int i = 0; i < count; i++) {
            cout << data[(frontIndex + i) % capacity];
            if (i < count - 1) cout << ", ";
        }
        cout << "]" << endl;
    }
};

// Замер времени выполнения лямбды в микросекундах
template <typename Func>
long long measureTime(Func func) {
    auto start = high_resolution_clock::now();
    func();
    auto end = high_resolution_clock::now();
    return duration_cast<microseconds>(end - start).count();
}

int main() {
    setlocale(LC_ALL, "ru");
    cout << "1. демонстрация Deque:" << endl;
    Deque<int> dq(4);
    dq.push_back(10);    // [10]
    dq.push_back(20);    // [10, 20]
    dq.push_front(5);    // [5, 10, 20]
    dq.push_front(1);    // [1, 5, 10, 20]
    dq.print();

    dq.push_back(30);    // вызов resize: [1, 5, 10, 20, 30]
    dq.print();

    cout << "pop_front: " << dq.pop_front() << endl;   // удаление 1
    cout << "pop_back:  " << dq.pop_back() << endl;   // удаление 30
    dq.print();                                        // [5, 10, 20]

    cout << "dq[1] = " << dq[1] << ", size = " << dq.size() << endl;

    // 2. Сравнение с std::deque
    cout << "\n2. сравнение с std::deque:" << endl;
    cout << "N\t\tcustom\t\tSTL" << endl;

    vector<int> sizes = { 10000, 50000, 100000 };
    for (int n : sizes) {
        Deque<int> custom;
        std::deque<int> stl;

        // тест Deque: push/pop с обоих концов
        long long tCustom = measureTime([&]() {
            for (int i = 0; i < n; i++) { custom.push_back(i); custom.push_front(i); }
            for (int i = 0; i < n; i++) { custom.pop_back();   custom.pop_front(); }
            });

        // тест для STL
        long long tSTL = measureTime([&]() {
            for (int i = 0; i < n; i++) { stl.push_back(i); stl.push_front(i); }
            for (int i = 0; i < n; i++) { stl.pop_back();   stl.pop_front(); }
            });

        cout << n << "\t\t" << tCustom << "\t\t" << tSTL << endl;
    }

    return 0;
}
