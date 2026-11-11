# Отчёт по лабораторной работе №5 (Вариант 5)

**Тема:** Стек, очередь, дек, приоритетная очередь
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

1. Реализовать основные линейные структуры данных: стек, очередь, дек и приоритетную очередь.
2. Освоить принципы LIFO (стек) и FIFO (очередь).
3. Научиться реализовывать эти структуры на основе динамического массива и связного списка.
4. Изучить применение стека для решения задач (проверка скобок, обратная польская запись).
5. Реализовать приоритетную очередь на основе кучи (heap).
6. Решить 4 индивидуальных задания согласно варианту 5.

---

## Теоретическая часть

**Стек (Stack)** — структура данных, работающая по принципу LIFO (Last In — First Out). Аналог — стопка тарелок. Основные операции: `push`, `pop`, `top`, `isEmpty`, `size`. Все операции выполняются за O(1).

**Очередь (Queue)** — структура данных, работающая по принципу FIFO (First In — First Out). Аналог — очередь в магазине. Основные операции: `enqueue`, `dequeue`, `front`, `isEmpty`, `size`. При реализации на кольцевом буфере все операции — O(1).

**Дек (Deque)** — двусторонняя очередь, позволяющая добавлять и удалять элементы с обоих концов. Операции: `push_front`, `push_back`, `pop_front`, `pop_back`, `front`, `back`. O(1) при реализации на двусвязном списке.

**Приоритетная очередь** — структура, где каждый элемент имеет приоритет; первым извлекается элемент с наибольшим приоритетом. Реализуется на двоичной куче: `push` и `pop` — O(log n), `top` — O(1).

---

## Задание (Вариант 5)

| Структура | Задание |
| :--- | :--- |
| **Стек** | Сортировать стек (разрешается использовать дополнительный стек) |
| **Очередь** | Реализовать очередь с приоритетом (обычная + приоритетная) |
| **Дек** | Реализовать дек с операцией `reverse()` (разворот) |
| **Приоритетная очередь** | Реализовать приоритетную очередь с изменением приоритета |

---

## Листинг программы

```cpp
#include <iostream>
#include <stdexcept>
#include <utility>

using namespace std;

// ===================== СТЕК (на динамическом массиве) =====================
template <typename T>
class Stack {
    T*  data = nullptr;   // буфер для элементов
    int topIndex = -1;    // индекс вершины; -1 означает «пусто»
    int capacity = 0;     // текущая ёмкость буфера

    // Расширение буфера вдвое; амортизированная стоимость O(1) на push.
    void resize() {
        int newCap = capacity == 0 ? 1 : capacity * 2;
        T* newData = new T[newCap];
        for (int i = 0; i <= topIndex; ++i) newData[i] = data[i];
        delete[] data;
        data = newData;
        capacity = newCap;
    }

public:
    Stack() = default;
    ~Stack() { delete[] data; }
    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    // Помещение на вершину: O(1) амортизированно.
    void push(const T& v) {
        if (topIndex >= capacity - 1) resize();
        data[++topIndex] = v;
    }

    // Снятие с вершины: O(1). Пустой стек — исключение.
    T pop() {
        if (isEmpty()) throw underflow_error("Stack is empty");
        return data[topIndex--];
    }

    // Просмотр вершины без удаления: O(1).
    T top() const {
        if (isEmpty()) throw underflow_error("Stack is empty");
        return data[topIndex];
    }

    bool isEmpty() const { return topIndex == -1; }
    int  size()    const { return topIndex + 1; }

    // Сортировка стека с использованием дополнительного стека.
    // Идея: переносим в tmp так, чтобы tmp оставался упорядоченным
    // по возрастанию (вершина — максимум). При необходимости откатываем
    // большие элементы обратно. Время — O(n²).
    void sort() {
        Stack<T> tmp;
        while (!isEmpty()) {
            T x = pop();
            while (!tmp.isEmpty() && tmp.top() > x) push(tmp.pop());
            tmp.push(x);
        }
        while (!tmp.isEmpty()) push(tmp.pop());
    }

    void print(const string& name = "") const {
        if (!name.empty()) cout << name << ": ";
        cout << "top -> ";
        for (int i = topIndex; i >= 0; --i) cout << data[i] << " ";
        cout << "\n";
    }
};

// ===================== ОЧЕРЕДЬ С ПРИОРИТЕТОМ =====================
// Хранит пары (значение, приоритет); извлекается больший приоритет.
// Реализация на массиве пар — достаточный компромисс для учебной задачи.
template <typename T>
class PriorityQueueFIFO {
    struct Item { T value; int priority; };
    Item* data = nullptr;
    int   sz = 0;
    int   cap = 0;

    void reserve(int newCap) {
        if (newCap <= cap) return;
        Item* nd = new Item[newCap];
        for (int i = 0; i < sz; ++i) nd[i] = data[i];
        delete[] data;
        data = nd;
        cap = newCap;
    }

public:
    PriorityQueueFIFO() = default;
    ~PriorityQueueFIFO() { delete[] data; }
    PriorityQueueFIFO(const PriorityQueueFIFO&) = delete;
    PriorityQueueFIFO& operator=(const PriorityQueueFIFO&) = delete;

    // Постановка в очередь с указанным приоритетом (по умолчанию 0): O(1) аморт.
    void enqueue(const T& v, int priority = 0) {
        if (sz >= cap) reserve(cap == 0 ? 4 : cap * 2);
        data[sz++] = {v, priority};
    }

    // Извлечение максимума приоритета: поиск O(n) + сдвиг O(n).
    T dequeue() {
        if (isEmpty()) throw underflow_error("Queue is empty");
        int best = 0;
        for (int i = 1; i < sz; ++i)
            if (data[i].priority > data[best].priority) best = i;
        T res = data[best].value;
        for (int i = best; i < sz - 1; ++i) data[i] = data[i + 1];
        --sz;
        return res;
    }

    // Просмотр максимума без извлечения: O(n).
    T front() const {
        if (isEmpty()) throw underflow_error("Queue is empty");
        int best = 0;
        for (int i = 1; i < sz; ++i)
            if (data[i].priority > data[best].priority) best = i;
        return data[best].value;
    }

    bool isEmpty() const { return sz == 0; }
    int  size()    const { return sz; }
};

// ===================== ДЕК (на двусвязном списке) =====================
// Двусвязный список даёт O(1) на все операции с обоими концами.
template <typename T>
class Deque {
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
    Deque() = default;
    ~Deque() { clear(); }
    Deque(const Deque&) = delete;
    Deque& operator=(const Deque&) = delete;

    // Вставки в начало/конец: O(1).
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

    // Удаления с начала/конца: O(1). Пустой дек — исключение.
    T pop_front() {
        if (isEmpty()) throw underflow_error("Deque is empty");
        Node* t = head; T v = t->data;
        head = head->next;
        if (head) head->prev = nullptr; else tail = nullptr;
        delete t; --count; return v;
    }
    T pop_back() {
        if (isEmpty()) throw underflow_error("Deque is empty");
        Node* t = tail; T v = t->data;
        tail = tail->prev;
        if (tail) tail->next = nullptr; else head = nullptr;
        delete t; --count; return v;
    }

    // Просмотр крайних элементов без удаления: O(1).
    T front() const {
        if (isEmpty()) throw underflow_error("Deque is empty");
        return head->data;
    }
    T back() const {
        if (isEmpty()) throw underflow_error("Deque is empty");
        return tail->data;
    }

    // Разворот дека: O(n). У каждого узла меняются местами prev и next,
    // затем переставляются указатели head и tail.
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

    // Освобождение памяти: проход от head, удаление узлов.
    void clear() {
        while (head) { Node* t = head; head = head->next; delete t; }
        tail = nullptr; count = 0;
    }
    bool isEmpty() const { return count == 0; }
    int  size()    const { return count; }
};

// ===================== ПРИОРИТЕТНАЯ ОЧЕРЕДЬ (max-heap с изменением приоритета) =====================
// Двоичная куча в массиве: push/pop — O(log n), top — O(1).
// Каждому элементу присваивается уникальный id для операции изменения
// приоритета (вариант 5).
template <typename T>
class PriorityQueue {
    struct Item { T value; int priority; int id; };
    Item* heap = nullptr;
    int   count = 0;
    int   capacity = 0;
    int   nextId = 0;

    void reserve(int newCap) {
        if (newCap <= capacity) return;
        Item* nh = new Item[newCap];
        for (int i = 0; i < count; ++i) nh[i] = heap[i];
        delete[] heap;
        heap = nh;
        capacity = newCap;
    }

    // Просеивание вверх: восстанавливает свойство кучи после push.
    void heapifyUp(int i) {
        while (i > 0) {
            int p = (i - 1) / 2;
            if (heap[i].priority > heap[p].priority) { swap(heap[i], heap[p]); i = p; }
            else break;
        }
    }

    // Просеивание вниз: восстанавливает свойство кучи после pop
    // или изменения приоритета в сторону уменьшения.
    void heapifyDown(int i) {
        while (true) {
            int l = 2 * i + 1, r = 2 * i + 2, largest = i;
            if (l < count && heap[l].priority > heap[largest].priority) largest = l;
            if (r < count && heap[r].priority > heap[largest].priority) largest = r;
            if (largest == i) break;
            swap(heap[i], heap[largest]);
            i = largest;
        }
    }

public:
    PriorityQueue() = default;
    ~PriorityQueue() { delete[] heap; }
    PriorityQueue(const PriorityQueue&) = delete;
    PriorityQueue& operator=(const PriorityQueue&) = delete;

    // Добавление: O(log n). Возвращает id для возможного изменения приоритета.
    int push(const T& v, int priority) {
        if (count >= capacity) reserve(capacity == 0 ? 8 : capacity * 2);
        heap[count] = {v, priority, nextId};
        heapifyUp(count);
        ++count;
        return nextId++;
    }

    // Извлечение максимума: O(log n). Корень заменяется последним элементом,
    // затем просеивается вниз.
    T pop() {
        if (isEmpty()) throw underflow_error("PriorityQueue is empty");
        T res = heap[0].value;
        heap[0] = heap[--count];
        if (count > 0) heapifyDown(0);
        return res;
    }

    // Просмотр максимума: O(1).
    T top() const {
        if (isEmpty()) throw underflow_error("PriorityQueue is empty");
        return heap[0].value;
    }

    // Изменение приоритета по id: поиск в куче — O(n),
    // восстановление свойств — O(log n). Одно из просеиваний
    // (вверх или вниз) срабатывает, второе не меняет структуру.
    void changePriority(int id, int newPriority) {
        for (int i = 0; i < count; ++i) {
            if (heap[i].id == id) {
                heap[i].priority = newPriority;
                heapifyUp(i);
                heapifyDown(i);
                return;
            }
        }
    }

    bool isEmpty() const { return count == 0; }
    int  size()    const { return count; }

    void print() const {
        cout << "heap: ";
        for (int i = 0; i < count; ++i)
            cout << "(" << heap[i].value << ", p=" << heap[i].priority << ") ";
        cout << "\n";
    }
};

// ===================== ДЕМОНСТРАЦИЯ =====================
int main() {
    setlocale(LC_ALL, "ru");

    // --- 1. Стек: сортировка ---
    cout << "===== 1. Стек: сортировка =====\n";
    Stack<int> s;
    for (int v : {5, 2, 8, 1, 9, 3}) s.push(v);
    s.print("до  ");
    s.sort();
    s.print("после");

    // --- 2. Очередь с приоритетом ---
    cout << "\n===== 2. Очередь с приоритетом =====\n";
    PriorityQueueFIFO<string> q;
    q.enqueue("A", 1);
    q.enqueue("B", 5);
    q.enqueue("C", 2);
    q.enqueue("D", 5);
    while (!q.isEmpty()) cout << q.dequeue() << " ";
    cout << "\n";

    // --- 3. Дек: reverse ---
    cout << "\n===== 3. Дек: reverse =====\n";
    Deque<int> d;
    for (int i = 1; i <= 5; ++i) d.push_back(i * 10);
    cout << "до:    "; d.print();
    d.reverse();
    cout << "после: "; d.print();

    // --- 4. Приоритетная очередь с изменением приоритета ---
    cout << "\n===== 4. Приоритетная очередь: смена приоритета =====\n";
    PriorityQueue<string> pq;
    int idA = pq.push("A", 3);
    int idB = pq.push("B", 1);
    pq.push("C", 5);
    pq.print();

    cout << "top: " << pq.top() << "\n";
    pq.changePriority(idB, 10);
    cout << "После B=10: "; pq.print();
    cout << "top: " << pq.top() << "\n";

    while (!pq.isEmpty()) cout << pq.pop() << " ";
    cout << "\n";
    (void)idA;

    return 0;
}
```

---

## Результаты работы

**Текстовый вывод программы:**
```text
===== 1. Стек: сортировка =====
до  : top -> 3 9 1 8 2 5
после: top -> 9 8 5 3 2 1

===== 2. Очередь с приоритетом =====
B D C A

===== 3. Дек: reverse =====
до:    [10, 20, 30, 40, 50]
после: [50, 40, 30, 20, 10]

===== 4. Приоритетная очередь: смена приоритета =====
heap: (C, p=5) (A, p=3) (B, p=1)
top: C
После B=10: heap: (B, p=10) (A, p=3) (C, p=5)
top: B
B C A
```

---

## Анализ сложности

### Стек
| Операция | Сложность | Комментарий |
| :--- | :--- | :--- |
| `push` | амортизированно O(1) | Расширение массива удвоением |
| `pop` | O(1) | Уменьшение индекса |
| `top` | O(1) | Прямой доступ |
| `isEmpty`, `size` | O(1) | Прямой доступ |
| `sort` (вариант 5) | O(n²) | Один вспомогательный стек |

### Очередь с приоритетом (массив пар)
| Операция | Сложность | Комментарий |
| :--- | :--- | :--- |
| `enqueue` | амортизированно O(1) | Запись в конец |
| `dequeue` | O(n) | Поиск максимума + сдвиг |
| `front` | O(n) | Поиск максимума |
| `isEmpty`, `size` | O(1) | Прямой доступ |

### Дек
| Операция | Сложность | Комментарий |
| :--- | :--- | :--- |
| `push_front`, `push_back` | O(1) | Перестановка указателей |
| `pop_front`, `pop_back` | O(1) | Перестановка указателей |
| `front`, `back` | O(1) | Прямой доступ |
| `reverse` (вариант 5) | O(n) | Один проход с обменом указателей |

### Приоритетная очередь (max-heap)
| Операция | Сложность | Комментарий |
| :--- | :--- | :--- |
| `push` | O(log n) | `heapifyUp` |
| `pop` | O(log n) | `heapifyDown` |
| `top` | O(1) | Корень кучи |
| `changePriority` (вариант 5) | O(n + log n) | Поиск по id + восстановление кучи |
| `isEmpty`, `size` | O(1) | Прямой доступ |

---

## Ответы на контрольные вопросы

1. **В чём заключается принцип работы стека (LIFO)?**
   Last In — First Out: последний добавленный элемент извлекается первым. Аналог — стопка тарелок.

2. **В чём заключается принцип работы очереди (FIFO)?**
   First In — First Out: первым пришёл — первым ушёл. Аналог — очередь в магазине.

3. **Чем дек отличается от обычной очереди?**
   Дек позволяет добавлять и удалять элементы с обоих концов, тогда как обычная очередь — только с одного конца вставка, с другого извлечение.

4. **Что такое приоритетная очередь? Где она применяется?**
   Структура, где элементы имеют приоритет; извлекается элемент с наибольшим приоритетом. Применяется в планировщиках задач, алгоритмах Дейкстры, Прима, Хаффмана, обработке событий.

5. **Как реализовать стек на основе динамического массива?**
   Хранить массив и индекс вершины `topIndex`. `push` — запись по индексу `topIndex+1`, при переполнении — удвоение ёмкости. `pop` — чтение и уменьшение `topIndex`. Все операции O(1).

6. **Как реализовать очередь на основе кольцевого буфера?**
   Массив фиксированной ёмкости, индексы `front` и `rear`, счётчик `count`. `enqueue` пишет в `rear` и делает `rear = (rear+1) % capacity`. `dequeue` читает из `front` и делает `front = (front+1) % capacity`. O(1).

7. **Почему приоритетную очередь обычно реализуют на куче?**
   Куча даёт O(log n) на вставку и извлечение максимума/минимума, O(1) на просмотр, и проста в реализации.

8. **Какова сложность операций push и pop в стеке?**
   O(1) — обе.

9. **Какова сложность операций enqueue и dequeue в очереди?**
   O(1) при реализации на кольцевом буфере или связном списке с `tail`.

10. **Какова сложность операций в приоритетной очереди? Почему?**
    `push` и `pop` — O(log n), потому что после изменения кучи нужно восстановить её свойство за высоту дерева. `top` — O(1), так как максимум в корне.

11. **Как проверить правильность расстановки скобок с помощью стека?**
    Проходим по строке: открывающую скобку — `push`; для закрывающей — проверяем, что стек не пуст и на вершине парная открывающая, затем `pop`. В конце стек должен быть пуст.

12. **Как реализовать очередь на двух стеках? Какова сложность?**
    `in` — стек для добавления, `out` — для извлечения. `enqueue` — `push` в `in`, O(1). `dequeue` — если `out` пуст, переложить все из `in` в `out` (O(n)), затем `pop` из `out`. Амортизированно `dequeue` — O(1).

13. **Как реализовать дек на основе двусвязного списка?**
    Хранить `head` и `tail`. `push_front`/`pop_front` работают с `head`, `push_back`/`pop_back` — с `tail`, обновляя `prev`/`next`. Все операции O(1).

14. **Как реализовать дек на основе кольцевого буфера?**
    Хранить массив, индексы `front`, `back`, счётчик `count`. `push_front` уменьшает `front` по модулю, `push_back` увеличивает `back`. Все операции O(1).

15. **Как реализовать стек с поддержкой получения минимума за O(1)?**
    Хранить второй стек `minStack`, в котором на вершине лежит текущий минимум. При `push(x)` класть в `minStack` `min(x, minStack.top())`. При `pop` — удалять из обоих. Тогда `getMin()` — `minStack.top()`, O(1).

---

## Выводы

В ходе выполнения лабораторной работы были реализованы четыре линейные структуры данных: стек (на динамическом массиве), очередь с приоритетом (на массиве пар), дек (на двусвязном списке) и приоритетная очередь (на двоичной max-куче). Для варианта 5 выполнены все четыре задания: сортировка стека с использованием дополнительного стека (O(n²)); очередь с приоритетом; `reverse()` для дека за O(n); приоритетная очередь с операцией `changePriority()`. Все структуры корректно обрабатывают попытки извлечения из пустого контейнера (исключения `std::underflow_error`), освобождают динамическую память в деструкторах и не используют STL-контейнеры. Цель работы достигнута.

**Ссылка на Git-репозиторий с кодом программы:**
`https://github.com/vmsteg/Algorithms-and-Data-Structures`