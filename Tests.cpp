#include "Tests.h"
#include <iostream>
#include <chrono>

#include "Ordinal.h"
#include "IGeneratorRule.h"
#include "FibonacciRule.h"
#include "LazySequence.h"
#include "SequenceStream.h"
#include "Exceptions.h"

namespace {
    // Вспомогательная функция из вашей 2-й лабы для красивого вывода
    void Assert(bool condition, const char* message) {
        if (!condition) {
            printf("[ FAIL ] %s\n", message);
            throw Exception(message);
        }
    }

    void PrintSuccess(const char* testName) {
        printf("[ OK ] %s\n", testName);
    }

    // Функторы для тестов
    class TestEvenFunctor : public IWhereFunction<size_t> {
    public:
        bool Check(size_t item) override { return item % 2 == 0; }
    };

    class TestSquareFunctor : public IMapFunction<size_t, size_t> {
    public:
        size_t Map(size_t item) override { return item * item; }
    };

    class TestSumFunctor : public IReduceFunction<size_t> {
    public:
        size_t Combine(size_t accum, size_t item) override { return accum + item; }
    };

    // Линейное правило для нагрузочного тестирования (0, 1, 2, 3...)
    class LinearRule : public IGeneratorRule<size_t> {
    public:
        size_t Generate(const Ordinal& idx) override { return idx.GetFinite(); }
        IGeneratorRule<size_t>* Clone() const override { return new LinearRule(); }
    };
}

void RunUnitTests() {
    std::cout << "--- Запуск автоматических модульных тестов ---\n";

    // 1. Базовые операции и ленивая генерация
    IGeneratorRule<size_t>* fibRule = new FibonacciRule();
    LazySequence<size_t> seq(fibRule, Ordinal(10)); // Ограничим длину 10 для тестов

    Assert(seq.GetFirst() == 0, "LazySequence: Ошибка GetFirst");
    Assert(seq.Get(1) == 1, "LazySequence: Ошибка Get(1)");
    Assert(seq.Get(4) == 3, "LazySequence: Ошибка Get(4)");
    Assert(seq.GetLast() == 34, "LazySequence: Ошибка GetLast");

    // Мутации
    seq.Append(999);
    Assert(seq.GetLast() == 999, "LazySequence: Ошибка Append");
    seq.RemoveLast();
    PrintSuccess("Базовые операции (Get, Append, RemoveLast)");

    // 2. Тестирование Where (Фильтрация четных)
    TestEvenFunctor isEven;
    LazySequence<size_t>* evens = seq.Where(&isEven);
    Assert(evens->GetFirst() == 0, "Where: Первый четный элемент неверен");
    Assert(evens->Get(1) == 2, "Where: Второй четный элемент неверен");
    Assert(evens->Get(2) == 8, "Where: Третий четный элемент неверен");
    PrintSuccess("Алгоритм Where (Фильтрация)");

    // 3. Тестирование Map (Возведение в квадрат)
    TestSquareFunctor squarer;
    LazySequence<size_t>* squares = seq.Map(&squarer);
    Assert(squares->Get(3) == 4, "Map: Квадрат 2 должен быть 4");
    Assert(squares->Get(4) == 9, "Map: Квадрат 3 должен быть 9");
    PrintSuccess("Алгоритм Map (Трансформация)");

    // 4. Тестирование Reduce (Свертка суммы)
    TestSumFunctor summer;
    // Первые 5 чисел Фибоначчи: 0 + 1 + 1 + 2 + 3 = 7
    Assert(seq.Reduce(&summer, 0, 5) == 7, "Reduce: Сумма первых 5 элементов неверна");
    PrintSuccess("Алгоритм Reduce (Свертка)");

        IGeneratorRule<size_t>* rule1 = new FibonacciRule();
        IGeneratorRule<size_t>* rule2 = new FibonacciRule();
        LazySequence<size_t> first(rule1, Ordinal(5));  // [0, 1, 1, 2, 3]
        LazySequence<size_t> second(rule2, Ordinal(5)); // [0, 1, 1, 2, 3]

        LazySequence<size_t>* concatSeq = first.Concat(&second);

        // Проверка длины: 5 + 5 = 10
        Assert(concatSeq->GetOrdinalLength().GetFinite() == 10, "Concat: Неверная длина результата");
        Assert(concatSeq->GetOrdinalLength().GetOmega() == 0, "Concat: Омега должна быть 0");

        // Элементы из первой половины
        Assert(concatSeq->Get(0) == 0, "Concat: Ошибка в первой части (индекс 0)");
        Assert(concatSeq->Get(4) == 3, "Concat: Ошибка на стыке первой части (индекс 4)");

        // Элементы из второй половины (проверка корректности сдвига индекса)
        Assert(concatSeq->Get(5) == 0, "Concat: Ошибка на стыке второй части (индекс 5)");
        Assert(concatSeq->Get(9) == 3, "Concat: Ошибка в конце второй части (индекс 9)");

        delete concatSeq;

        // 2. Проверка трансфинитной конкатенации: w + w = 2w
        IGeneratorRule<size_t>* infRule1 = new FibonacciRule();
        IGeneratorRule<size_t>* infRule2 = new FibonacciRule();
        LazySequence<size_t> inf1(infRule1); // длина 1*w
        LazySequence<size_t> inf2(infRule2); // длина 1*w

        LazySequence<size_t>* doubleInf = inf1.Concat(&inf2);
        Assert(doubleInf->GetOrdinalLength().GetOmega() == 2, "Concat: Длина w + w должна давать 2*w");
        
        // Чтение из второй омеги через GetByOrdinal
        Assert(doubleInf->GetByOrdinal(Ordinal(1, 4)) == 3, "Concat: Ошибка чтения из 2-й бесконечности");

        delete doubleInf;
        PrintSuccess("Алгоритм Concat (Конечная и 2w конкатенация)");

    // ---------------------------------------------------------
    // Тестирование алгебры ленивых списков и ординалов (Вариант 2)
    // ---------------------------------------------------------
    {
        IGeneratorRule<size_t>* r1 = new FibonacciRule();
        IGeneratorRule<size_t>* r2 = new FibonacciRule();
        IGeneratorRule<size_t>* r3 = new FibonacciRule();
        LazySequence<size_t> a(r1, Ordinal(3)); // [0, 1, 1]
        LazySequence<size_t> b(r2, Ordinal(4)); // [0, 1, 1, 2]
        LazySequence<size_t> c(r3, Ordinal(2)); // [0, 1]

        // 5. НЕКОММУТАТИВНОСТЬ ординалов (1 + w == w, но w + 1 > w)
        Ordinal w = Ordinal::Omega();
        Ordinal one = Ordinal::Finite(1);
        Assert((w + one) != (one + w), "LazyAlgebra: Ординалы некоммутативны (w+1 != 1+w)");

        PrintSuccess("Алгебра ленивых списков и ординалов");
    }

    // 5. Тестирование SequenceStream
    SequenceStream<size_t> testStream(&seq);
    bool caught = false;
    try {
        testStream.Read(); // Чтение из закрытого потока
    } catch (const Exception&) { caught = true; }
    catch (const std::exception&) { caught = true; } // На случай std::exception
    Assert(caught, "Stream (Neg): Нет исключения при чтении закрытого потока");

    testStream.Open();
    Assert(!testStream.IsEndOfStream(), "Stream: Поток не должен быть пуст");
    Assert(testStream.Read() == 0, "Stream: Ошибка чтения первого элемента");
    Assert(testStream.Read() == 1, "Stream: Ошибка чтения второго элемента");
    
    testStream.Seek(5);
    Assert(testStream.Read() == 5, "Stream: Ошибка Seek()");
    testStream.Close();
    PrintSuccess("Потоковое чтение (SequenceStream)");

    // 6. Негативные тесты для Sequence
    caught = false;
    try {
        seq.Get(100); // Выход за границы (у нас длина 10 + 1 Append - 1 Remove = 10)
    } catch (const Exception&) { caught = true; }
    Assert(caught, "LazySequence (Neg): Нет исключения при выходе за границы");

    caught = false;
    try {
        seq.Concat(nullptr);
    } catch (const Exception&) { caught = true; }
    Assert(caught, "LazySequence (Neg): Нет исключения при Concat(nullptr)");
    PrintSuccess("Негативные сценарии (Exceptions)");

    delete evens;
    delete squares;
}

void RunStressTest() {
    std::cout << "\n--- Нагрузочное тестирование ---\n";
    std::cout << "Генерация 1 000 000 элементов и потоковая обработка\n";

    auto start = std::chrono::high_resolution_clock::now();

    IGeneratorRule<size_t>* rule = new LinearRule();
    LazySequence<size_t> bigSeq(rule); // По умолчанию длина Omega (бесконечная)

    SequenceStream<size_t> stream(&bigSeq);
    stream.Open();
    
    size_t streamSum = 0;
    const int limit = 1000000;
    
    for(int i = 0; i < limit; ++i) {
        streamSum += stream.Read();
    }
    stream.Close();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Успешно обработан 1 000 000 элементов через Stream.\n";
    std::cout << "Затраченное время: " << duration.count() << " секунд.\n";
}

void RunAllTests() {
    try {
        RunUnitTests();
        RunStressTest();
        std::cout << "\n========================================\n";
        std::cout << "       ВСЕ ТЕСТЫ УСПЕШНО ПРОЙДЕНЫ!      \n";
        std::cout << "========================================\n";
    } catch (const Exception& e) {
        std::cout << "\n[КРИТИЧЕСКАЯ ОШИБКА ТЕСТА]: " << e.GetMessage() << "\n";
    } catch (...) {
        std::cout << "\n[КРИТИЧЕСКАЯ ОШИБКА ТЕСТА]: Неизвестное исключение\n";
    }
}