#pragma once
#include <cassert>
#include <cstddef>
#include <utility>
#include <string>
#include <iterator>

template <typename Type>
class SingleLinkedList {

    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };

    // Шаблон класса «Базовый Итератор».
    // Определяет поведение итератора на элементы односвязного списка
    // ValueType — совпадает с Type (для Iterator) либо с const Type (для ConstIterator)
    template <typename ValueType>
    class BasicIterator {
        // Класс списка объявляется дружественным, чтобы из методов списка
        // был доступ к приватной области итератора
        friend class SingleLinkedList;

        // Конвертирующий конструктор итератора из указателя на узел списка
        explicit BasicIterator(Node* node) {
            node_ = node;
        }

    public:
        // Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора

        // Категория итератора — forward iterator
        // (итератор, который поддерживает операции инкремента и многократное разыменование)
        using iterator_category = std::forward_iterator_tag;
        // Тип элементов, по которым перемещается итератор
        using value_type = Type;
        // Тип, используемый для хранения смещения между итераторами
        using difference_type = std::ptrdiff_t;
        // Тип указателя на итерируемое значение
        using pointer = ValueType*;
        // Тип ссылки на итерируемое значение
        using reference = ValueType&;

        BasicIterator() = default;

        // Конвертирующий конструктор/конструктор копирования
        // При ValueType, совпадающем с Type, играет роль копирующего конструктора
        // При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
        BasicIterator(const BasicIterator<Type>& other) noexcept {
            node_ = other.node_;

        }

        // Чтобы компилятор не выдавал предупреждение об отсутствии оператора = при наличии
        // пользовательского конструктора копирования, явно объявим оператор = и
        // попросим компилятор сгенерировать его за нас
        BasicIterator& operator=(const BasicIterator& rhs) = default;

        // Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            if (node_ == rhs.node_) {
                return true;
            }
            else {
                return false;
            }
        }

        // Оператор проверки итераторов на неравенство
        // Противоположен !=
        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            if (node_ != rhs.node_) {
                return true;
            }
            else {
                return false;
            }
        }

        // Оператор сравнения итераторов (в роли второго аргумента итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            if (node_ == rhs.node_) {
                return true;
            }
            else {
                return false;
            }
        }

        // Оператор проверки итераторов на неравенство
        // Противоположен !=
        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            if (node_ != rhs.node_) {
                return true;
            }
            else {
                return false;
            }
        }

        // Оператор прединкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает ссылку на самого себя
        // Инкремент итератора, не указывающего на существующий элемент списка, приводит к неопределённому поведению
        BasicIterator& operator++() noexcept {
            node_ = node_->next_node;
            return *this;
        }

        // Оператор постинкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает прежнее значение итератора
        // Инкремент итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        BasicIterator operator++(int) noexcept {
            auto old_value(*this);
            ++(*this);
            return old_value;
        }

        // Операция разыменования. Возвращает ссылку на текущий элемент
        // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] reference operator*() const noexcept {
            return node_->value;
        }

        // Операция доступа к члену класса. Возвращает указатель на текущий элемент списка
        // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] pointer operator->() const noexcept {
            return &(node_->value);
        }

    private:
        Node* node_ = nullptr;
    };

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    // Итератор, допускающий изменение элементов списка
    using Iterator = BasicIterator<Type>;
    // Константный итератор, предоставляющий доступ для чтения к элементам списка
    using ConstIterator = BasicIterator<const Type>;

    SingleLinkedList(std::initializer_list<Type> values) {
        Node* last = nullptr;
        for (auto i = values.begin(); i != values.end(); i++) {
            Node* node = new Node(*i, nullptr);
            if (!(head_.next_node)) {
                head_.next_node = node;
            }
            else {
                last->next_node = node;
            }
            last = node;
            size_++;
        }
    }

    SingleLinkedList(const SingleLinkedList& other) {
        Node* last = nullptr;
        for (Node* n = other.head_.next_node; n != nullptr; n = n->next_node) {
            Node* node = new Node(n->value, nullptr);
            if (!(head_.next_node)) {
                head_.next_node = node;
            }
            else {
                last->next_node = node;
            }
            last = node;
            size_++;
        }
    }

    // Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator{ &head_ };
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator{ const_cast<Node*>(&head_) };
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator{ &head_ };
    }

    /*
     * Вставляет элемент value после элемента, на который указывает pos.
     * Возвращает итератор на вставленный элемент
     * Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
     */
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        auto temp = pos;
        pos++;
        Node* node = new Node(value, pos.node_);
        temp.node_->next_node = node;
        ++size_;
        return Iterator{ node };
    }

    void PopFront() noexcept {
        Node* node = new Node(head_.next_node->value, head_.next_node);
        head_.next_node = head_.next_node->next_node;
        --size_;
        delete node;
    }

    /*
     * Удаляет элемент, следующий за pos.
     * Возвращает итератор на элемент, следующий за удалённым
     */
    Iterator EraseAfter(ConstIterator pos) noexcept {
        if (pos.node_->next_node == nullptr) {
            return Iterator{};
        }
        auto temp = pos.node_->next_node;
        pos.node_->next_node = pos.node_->next_node->next_node;
        delete temp;
        --size_;
        return Iterator{ pos.node_->next_node };
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (this != &rhs) {
            auto ths_copy(rhs);
            this->Clear();

            Node* last = nullptr;
            for (auto i = rhs.begin(); i != rhs.end(); i++) {
                Node* node = new Node(*i, nullptr);
                if (!(head_.next_node)) {
                    head_.next_node = node;
                }
                else {
                    last->next_node = node;
                }
                last = node;
                size_++;
            }
        }
        return *this;
    }

    void swap(SingleLinkedList& other) noexcept {
        auto size = this->size_;
        auto node = this->head_.next_node;

        this->head_.next_node = other.head_.next_node;
        this->size_ = other.size_;

        other.head_.next_node = node;
        other.size_ = size;
    }

    // Возвращает итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    [[nodiscard]] Iterator begin() noexcept {
        if (head_.next_node != nullptr) {
            return Iterator{ head_.next_node };
        }
        else {
            return Iterator{};
        }
    }

    // Возвращает итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator end() noexcept {
        if (head_.next_node != nullptr) {
            auto temp = head_.next_node;
            while (temp != nullptr) {
                temp = temp->next_node;
            }
            return Iterator{ temp };
        }
        else {
            return Iterator{};
        }
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    // Результат вызова эквивалентен вызову метода cbegin()
    [[nodiscard]] ConstIterator begin() const noexcept {
        if (head_.next_node != nullptr) {
            return ConstIterator{ head_.next_node };
        }
        else {
            return ConstIterator{};
        }
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    // Результат вызова эквивалентен вызову метода cend()
    [[nodiscard]] ConstIterator end() const noexcept {
        if (head_.next_node != nullptr) {
            auto temp = head_.next_node;
            while (temp != nullptr) {
                temp = temp->next_node;
            }
            return ConstIterator{ temp };
        }
        else {
            return ConstIterator{};
        }
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен cend()
    [[nodiscard]] ConstIterator cbegin() const noexcept {
        if (head_.next_node != nullptr) {
            return ConstIterator{ head_.next_node };
        }
        else {
            return ConstIterator{};
        }
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cend() const noexcept {
        if (head_.next_node != nullptr) {
            auto temp = head_.next_node;
            while (temp != nullptr) {
                temp = temp->next_node;
            }
            return ConstIterator{ temp };
        }
        else {
            return ConstIterator{};
        }
    }

    SingleLinkedList() = default;

    ~SingleLinkedList() {
        Clear();
    }

    // Вставляет элемент value в начало списка за время O(1)
    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    // Очищает список за время O(N)
    void Clear() noexcept {
        while (size_ != 0) {
            Node* p = head_.next_node;
            head_.next_node = head_.next_node->next_node;
            delete p;
            size_--;
        }
    }

    // Возвращает количество элементов в списке
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    // Сообщает, пустой ли список
    [[nodiscard]] bool IsEmpty() const noexcept {
        return !GetSize();
    }

private:
    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_ = Node();
    size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    auto it_l_begin = lhs.begin();
    auto it_r_begin = rhs.begin();
    auto it_l_end = lhs.end();
    auto it_r_end = rhs.end();

    for (; (it_l_begin != it_l_end) && (it_r_begin != it_r_end); ++it_l_begin, (void) ++it_r_begin)
    {
        if (*it_l_begin != *it_r_begin) {
            return false;
        }
    }
    return true;
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    auto it_l_begin = lhs.begin();
    auto it_r_begin = rhs.begin();
    auto it_l_end = lhs.end();
    auto it_r_end = rhs.end();

    for (; (it_l_begin != it_l_end) && (it_r_begin != it_r_end); ++it_l_begin, (void) ++it_r_begin)
    {
        if (*it_l_begin < *it_r_begin)
            return true;
        if (*it_r_begin < *it_l_begin)
            return false;
    }

    return (it_l_begin == it_l_end) && (it_r_begin != it_r_end);
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    if (lhs == rhs) {
        return true;
    }
    if (lhs < rhs) {
        return true;
    }
    return false;
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    if (lhs == rhs) {
        return true;
    }
    if (rhs < lhs) {
        return true;
    }
    return false;
}