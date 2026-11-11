# Отчёт по лабораторной работе №4 (Вариант 5)

**Тема:** Односвязный и двусвязный списки
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

1. Реализовать шаблонный класс `SinglyLinkedList` (односвязный список) с базовыми операциями.
2. Реализовать шаблонный класс `DoublyLinkedList` (двусвязный список) с базовыми операциями.
3. Освоить операции вставки, удаления, поиска и обхода элементов в списках.
4. Сравнить производительность и особенности использования односвязного и двусвязного списков.
5. Научиться корректно управлять памятью при работе со списками (удаление узлов, отсутствие утечек).
6. Реализовать дополнительную функциональность согласно варианту 5: **метод `mergeSort()` для односвязного списка** (сортировка слиянием за O(n log n)).

---

## Теоретическая часть

**Связный список** — структура данных, в которой элементы (узлы) хранятся в памяти не непрерывно. Каждый узел содержит данные и указатель(и) на следующий (и/или предыдущий) узел. Ключевые поля класса-контейнера: `head` — указатель на первый узел, `tail` — на последний, `count` — число элементов.

**Односвязный список** хранит в узле только указатель `next`. Это даёт O(1) на вставку/удаление в начале, но O(n) на удаление с конца (нельзя за O(1) найти предпоследний узел). Хранение `tail` ускоряет `push_back` до O(1).

**Двусвязный список** хранит в узле два указателя — `prev` и `next`. Это обеспечивает O(1) на удаление с конца и на удаление произвольного известного узла, а также обход в обе стороны. Цена — дополнительная память на узел (+8 байт на x64).

**Сортировка слиянием (`mergeSort`)** — алгоритм «разделяй и властвуй»: список делится пополам (методом двух указателей slow/fast), половины рекурсивно сортируются и сливаются. Сложность O(n log n), дополнительная память не выделяется (только стек рекурсии O(log n)).

---

## Задание (продвинутый уровень)

**Общее задание:** реализовать оба класса (`SinglyLinkedList` и `DoublyLinkedList`) с полной функциональностью и дополнительными требованиями.

**Дополнительное требование (Вариант 5):** реализовать метод `mergeSort()` для односвязного списка.

**Дополнительно реализовано:** итераторы для двусвязного списка (обход в стиле STL), метод `reverse()`, точечные операции `insert_after` / `erase` за O(1).

---

## Листинг программы

```cpp
#include <iostream>
#include <stdexcept>
#include <utility>

using namespace std;

// ===================== ОДНОСВЯЗНЫЙ СПИСОК =====================
// Узел содержит значение и указатель на следующий элемент.
template <typename T>
class SinglyLinkedList {
    struct Node {
        T data;
        Node* next;
        explicit Node(const T& v) : data(v), next(nullptr) {}
    };

    Node* head = nullptr;   // первый узел
    Node* tail = nullptr;   // последний узел — O(1) для push_back
    int   count = 0;        // число элементов

    // ---------- Вспомогательные методы сортировки слиянием ----------

    // Разделение списка пополам: slow — 1 шаг, fast — 2.
    static Node* split(Node* src) {
        Node* slow = src;
        Node* fast = src->next;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }
        Node* second = slow->next;
        slow->next = nullptr;           // разрыв связи
        return second;
    }

    // Слияние двух упорядоченных списков
    static Node* merge(Node* a, Node* b) {
        if (!a) return b;
        if (!b) return a;
        if (a->data <= b->data) {
            a->next = merge(a->next, b);
            return a;
        }
        b->next = merge(a, b->next);
        return b;
    }

    // Рекурсивная сортировка: базовый случай — 0/1 узел
    static Node* sortRec(Node* n) {
        if (!n || !n->next) return n;
        Node* second = split(n);
        return merge(sortRec(n), sortRec(second));
    }

public:
    SinglyLinkedList() = default;
    ~SinglyLinkedList() { clear(); }

    // Копирование запрещено — иначе двойное освобождение узлов.
    SinglyLinkedList(const SinglyLinkedList&) = delete;
    SinglyLinkedList& operator=(const SinglyLinkedList&) = delete;

    // Вставка в начало: O(1).
    void push_front(const T& v) {
        Node* n = new Node(v);
        n->next = head;
        head = n;
        if (!tail) tail = n;
        ++count;
    }

    // Вставка в конец: O(1)
    void push_back(const T& v) {
        Node* n = new Node(v);
        if (!head) head = tail = n;
        else { tail->next = n; tail = n; }
        ++count;
    }

    // Удаление из начала: O(1).
    void pop_front() {
        if (!head) throw underflow_error("empty");
        Node* t = head;
        head = head->next;
        delete t;
        --count;
        if (!head) tail = nullptr;
    }

    // Удаление из конца: O(n)
    void pop_back() {
        if (!head) throw underflow_error("empty");
        if (!head->next) {
            delete head;
            head = tail = nullptr;
        } else {
            Node* cur = head;
            while (cur->next != tail) cur = cur->next;
            delete tail;
            tail = cur;
            tail->next = nullptr;
        }
        --count;
    }

    // Поиск по значению: O(n).
    Node* find(const T& v) const {
        for (Node* cur = head; cur; cur = cur->next)
            if (cur->data == v) return cur;
        return nullptr;
    }

    // Сортировка слиянием: O(n log n), без доп. памяти (кроме стека).
    void mergeSort() {
        if (!head || !head->next) return;
        head = sortRec(head);
        tail = head;
        while (tail->next) tail = tail->next;
    }

    // Вывод: [a, b, c].
    void print() const {
        cout << "[";
        for (Node* cur = head; cur; cur = cur->next) {
            cout << cur->data;
            if (cur->next) cout << ", ";
        }
        cout << "]\n";
    }

    // Освобождение памяти и сброс состояния.
    void clear() {
        while (head) { Node* t = head; head = head->next; delete t; }
        tail = nullptr;
        count = 0;
    }

    int  size()  const { return count; }
    bool empty() const { return head == nullptr; }
    Node* getHead() const { return head; }
};

// ===================== ДВУСВЯЗНЫЙ СПИСОК =====================
// prev обеспечивает O(1) на удаление с конца и удаление известного узла.
template <typename T>
class DoublyLinkedList {
    struct Node {
        T data;
        Node* prev;
        Node* next;
        explicit Node(const T& v) : data(v), prev(nullptr), next(nullptr) {}
    };

    Node* head = nullptr;
    Node* tail = nullptr;
    int   count = 0;

public:
    // Минимальный прямой итератор
    class Iterator {
        Node* ptr;
    public:
        explicit Iterator(Node* p = nullptr) : ptr(p) {}
        T& operator*() { return ptr->data; }
        Iterator& operator++() { ptr = ptr->next; return *this; }
        bool operator==(const Iterator& o) const { return ptr == o.ptr; }
        bool operator!=(const Iterator& o) const { return ptr != o.ptr; }
    };

    DoublyLinkedList() = default;
    ~DoublyLinkedList() { clear(); }
    DoublyLinkedList(const DoublyLinkedList&) = delete;
    DoublyLinkedList& operator=(const DoublyLinkedList&) = delete;

    void push_front(const T& v) {
        Node* n = new Node(v);
        if (!head) head = tail = n;
        else { n->next = head; head->prev = n; head = n; }
        ++count;
    }

    void push_back(const T& v) {
        Node* n = new Node(v);
        if (!tail) head = tail = n;
        else { tail->next = n; n->prev = tail; tail = n; }
        ++count;
    }

    // Удаления: обе O(1)
    void pop_front() {
        if (!head) throw underflow_error("empty");
        Node* t = head;
        head = head->next;
        if (head) head->prev = nullptr;
        else      tail = nullptr;
        delete t;
        --count;
    }

    void pop_back() {
        if (!tail) throw underflow_error("empty");
        Node* t = tail;
        tail = tail->prev;
        if (tail) tail->next = nullptr;
        else      head = nullptr;
        delete t;
        --count;
    }

    // Поиск: O(n).
    Node* find(const T& v) const {
        for (Node* cur = head; cur; cur = cur->next)
            if (cur->data == v) return cur;
        return nullptr;
    }

    // Вставка после узла: O(1).
    void insert_after(Node* node, const T& v) {
        if (!node) { push_front(v); return; }
        Node* n = new Node(v);
        n->next = node->next;
        n->prev = node;
        if (node->next) node->next->prev = n;
        else            tail = n;
        node->next = n;
        ++count;
    }

    // Удаление узла: O(1);
    void erase(Node* node) {
        if (!node) return;
        if (node == head) { pop_front(); return; }
        if (node == tail) { pop_back();  return; }
        node->prev->next = node->next;
        node->next->prev = node->prev;
        delete node;
        --count;
    }

    // Реверс: O(n)
    void reverse() {
        for (Node* cur = head; cur; ) {
            Node* nxt = cur->next;
            swap(cur->prev, cur->next);
            cur = nxt;
        }
        swap(head, tail);
    }

    void print() const {
        cout << "[";
        for (Node* cur = head; cur; cur = cur->next) {
            cout << cur->data;
            if (cur->next) cout << ", ";
        }
        cout << "]\n";
    }

    void printReverse() const {
        cout << "[";
        for (Node* cur = tail; cur; cur = cur->prev) {
            cout << cur->data;
            if (cur->prev) cout << ", ";
        }
        cout << "]\n";
    }

    void clear() {
        while (head) { Node* t = head; head = head->next; delete t; }
        tail = nullptr;
        count = 0;
    }

    int  size()  const { return count; }
    bool empty() const { return head == nullptr; }

    Iterator begin() { return Iterator(head); }
    Iterator end()   { return Iterator(nullptr); }
};

// ===================== ДЕМОНСТРАЦИЯ =====================
int main() {
    setlocale(LC_ALL, "ru");

    // --- 1. Односвязный список: mergeSort ---
    cout << "===== SinglyLinkedList: mergeSort =====\n";
    SinglyLinkedList<int> s;
    for (int v : {42, 7, 19, 3, 88, 15, 1, 56, 24, 9}) s.push_back(v);

    cout << "До:     "; s.print();
    s.mergeSort();
    cout << "После:  "; s.print();

    // Проверка корректности tail после сортировки.
    s.push_back(100);
    cout << "+100:   "; s.print();

    // --- 2. Двусвязный список: итератор, reverse, O(1)-операции ---
    cout << "\n===== DoublyLinkedList: итератор, reverse =====\n";
    DoublyLinkedList<int> d;
    for (int i = 1; i <= 6; ++i) d.push_back(i * 10);

    cout << "Прямой:   "; d.print();
    cout << "Обратный: "; d.printReverse();

    cout << "Обход:    ";
    for (auto it = d.begin(); it != d.end(); ++it) cout << *it << " ";
    cout << "\n";

    d.reverse();
    cout << "Reverse:  "; d.print();

    if (auto* n = d.find(30)) d.insert_after(n, 999);
    cout << "insert:   "; d.print();
    if (auto* n = d.find(999)) d.erase(n);
    cout << "erase:    "; d.print();

    return 0;
}
```

---

## Результаты работы

**Текстовый вывод программы:**
```text
===== SinglyLinkedList: mergeSort =====
До:     [42, 7, 19, 3, 88, 15, 1, 56, 24, 9]
После:  [1, 3, 7, 9, 15, 19, 24, 42, 56, 88]
+100:   [1, 3, 7, 9, 15, 19, 24, 42, 56, 88, 100]

===== DoublyLinkedList: итератор, reverse =====
Прямой:   [10, 20, 30, 40, 50, 60]
Обратный: [60, 50, 40, 30, 20, 10]
Обход:    10 20 30 40 50 60
Reverse:  [60, 50, 40, 30, 20, 10]
insert:   [60, 50, 40, 30, 999, 20, 10]
erase:    [60, 50, 40, 30, 20, 10]
```

---

## Анализ сложности

### Односвязный список

| Операция | Сложность | Комментарий |
| :--- | :--- | :--- |
| `push_front` | O(1) | Перестановка указателей |
| `push_back` | O(1) | Благодаря указателю `tail` |
| `pop_front` | O(1) | Удаление головы |
| `pop_back` | O(n) | Нет `prev`, поиск предпоследнего узла |
| `find` | O(n) | Линейный проход |
| `mergeSort` | **O(n log n)** | log n уровней деления, слияние O(n) на уровень |
| `print`, `clear` | O(n) | Полный обход |
| Память на узел | данные + 1 указатель | — |

### Двусвязный список

| Операция | Сложность | Комментарий |
| :--- | :--- | :--- |
| `push_front` / `push_back` | O(1) | Перестановка указателей |
| `pop_front` / `pop_back` | O(1) | Наличие `prev` |
| `find` | O(n) | Линейный проход |
| `insert_after(node)` | O(1) | Перестановка четырёх указателей |
| `erase(node)` | O(1) | Наличие `prev` и `next` |
| `reverse` | O(n) | Один проход с обменом указателей |
| `print`, `printReverse`, `clear` | O(n) | Полный обход |
| Память на узел | данные + 2 указателя | +8 байт на x64 относительно односвязного |

---

## Ответы на контрольные вопросы

1. **Чем связный список отличается от динамического массива?**
   Массив хранит элементы непрерывно: доступ по индексу O(1), но вставка/удаление в середину — O(n) из-за сдвига. Список хранит узлы вразброс, связанные указателями: доступ по индексу O(n), но вставка/удаление при известном узле — O(1). Список не требует перевыделения и копирования при росте.

2. **Какие операции в односвязном списке выполняются за O(1)?**
   `push_front`, `push_back` (при наличии `tail`), `pop_front`, `insert_after(node)`, `size`, `empty`.

3. **Почему удаление из конца односвязного списка требует O(n)?**
   Нет указателя `prev`. Чтобы удалить последний узел, нужно обнулить `next` у предпоследнего, а к нему можно добраться только проходом от `head` до `tail` — O(n).

4. **Какие преимущества даёт хранение указателя `tail`?**
   Позволяет выполнять `push_back` за O(1) вместо O(n), быстро получать последний элемент, эффективно строить очередь на односвязном списке.

5. **Чем двусвязный список отличается от односвязного?**
   У каждого узла два указателя (`prev` и `next`). Это даёт O(1) удаление с конца, O(1) удаление известного узла, обход в обе стороны, `reverse` за O(n) без рекурсии. Цена — +1 указатель памяти на узел.

6. **Какие операции в двусвязном списке быстрее, чем в односвязном?**
   `pop_back` (O(1) против O(n)), `erase(node)` (O(1) против O(n)), обход в обратном направлении.

7. **Что такое «голова» (head) и «хвост» (tail) списка?**
   `head` — указатель на первый узел, `tail` — на последний. По ним определяются границы списка; `head == nullptr` означает пустой список.

8. **Как удалить узел из середины односвязного списка?**
   Найти узел перед удаляемым (`current->next == target`), затем `current->next = target->next; delete target;`. Если удаляется `tail`, обновить `tail = current`. O(n) из-за поиска предшественника.

9. **Как удалить узел из середины двусвязного списка? Почему это проще?**
   `node->prev->next = node->next; node->next->prev = node->prev; delete node;` — O(1) без поиска предшественника, потому что `prev` уже есть.

10. **Что произойдёт, если забыть обновить `tail` при удалении?**
    `tail` будет указывать на освобождённую память (висячий указатель). Последующие `push_back` / `print` / деструктор приведут к неопределённому поведению или падению.

11. **Как реализовать реверс односвязного списка?**
    Итеративно: три указателя `prev = nullptr`, `cur = head`, `next`; в цикле `next = cur->next; cur->next = prev; prev = cur; cur = next;`; в конце `head = prev`. O(n), O(1) памяти.

12. **Как обнаружить цикл в связном списке?**
    Алгоритм «черепахи и зайца» (Флойд): два указателя, один идёт на 1 шаг, другой на 2; если встретились — цикл есть. O(n), O(1).

13. **В каких случаях стоит использовать список вместо массива?**
    Частые вставки/удаления в начале или середине при известном узле; не требуется произвольный доступ по индексу; размер сильно меняется; элементы дорого копировать; нужна стабильность указателей/итераторов при модификациях.

14. **Какая дополнительная память требуется для каждого узла списка?**
    Односвязный: 1 указатель `next` (8 байт на x64). Двусвязный: 2 указателя `prev` и `next` (16 байт на x64). Плюс накладные расходы аллокатора.

15. **Почему доступ по индексу в списке имеет сложность O(n)?**
    Узлы не лежат подряд, нет арифметики указателей. Чтобы добраться до i-го элемента, нужно пройти i указателей от `head`. Поэтому O(n).

---

## Выводы

В ходе выполнения лабораторной работы были реализованы шаблонные классы `SinglyLinkedList` и `DoublyLinkedList` с корректным управлением памятью (деструкторы, `clear`, запрет копирования). Для односвязного списка реализован **`mergeSort()` за O(n log n)** — сортировка слиянием без дополнительной памяти, с восстановлением `tail` после перестройки связей. Для двусвязного списка реализованы итераторы (обход в стиле STL), `reverse`, `insert_after` и `erase` за O(1). Проведено сравнение производительности: двусвязный список выигрывает на операциях с конца и удалении известного узла, но расходует больше памяти на узел. Цель работы достигнута в полном объёме.

**Ссылка на Git-репозиторий с кодом программы:**
`https://github.com/vmsteg/Algorithms-and-Data-Structures`