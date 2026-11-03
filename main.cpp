#include <iostream>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "LazySequence.h"
#include "SequenceStream.h"
#include "FibonacciRule.h"
#include "Functors.h"
#include "Exceptions.h"
#include "Tests.h"



class MultiplyByTwoFunctor : public IMapFunction<size_t, size_t> {
public:
    size_t Map(size_t item) override { 
        return item * 2; 
    }
};

class IsEvenFunctor : public IWhereFunction<size_t> {
public:
    bool Check(size_t item) override { 
        return item % 2 == 0; 
    }
};

class SumFunctor : public IReduceFunction<size_t> {
public:
    size_t Combine(size_t accum, size_t item) override { 
        return accum + item; 
    }
};



int ReadInt(const char* prompt) {
    printf("%s", prompt);
    int value = 0;
    while (scanf("%d", &value) != 1) {
        while (getchar() != '\n'); 
        printf("Ошибка ввода. Ожидается число.\n%s", prompt);
    }
    return value;
}

void PrintSequenceElements(LazySequence<size_t>* seq, size_t count) {
    printf("Элементы [0 - %zu]: [ ", count - 1);
    for (size_t i = 0; i < count; ++i) {
        printf("%zu ", seq->Get(i));
    }
    printf("]\n");
}



int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    
    IGeneratorRule<size_t>* baseRule = new FibonacciRule();
    LazySequence<size_t>* currentSeq = new LazySequence<size_t>(baseRule);

    MultiplyByTwoFunctor multiplier;
    IsEvenFunctor evenFilter;
    SumFunctor summer;

    int choice = -1;

    while (choice != 0) {
        printf("   Меню для работы с ленивыми последовательностями   \n");
        printf("Текущая длина (Ординал): %zu бесконечностей + %zu элементов\n", 
               currentSeq->GetOrdinalLength().GetOmega(), 
               currentSeq->GetOrdinalLength().GetFinite());
        printf("Материализовано в кэше: %zu элементов\n", currentSeq->GetMaterializedCount());
        printf("------------------------------------------------------\n");
        printf("1. Вывести первые N элементов (Get)\n");
        printf("2. Получить элемент по индексу (operator[])\n");
        printf("3. Добавить элемент в конец (Append)\n");
        printf("4. Добавить элемент в начало (Prepend)\n");
        printf("5. Вставить элемент по индексу (InsertAt)\n");
        printf("6. Удалить последний элемент (RemoveLast)\n");
        printf("7. Применить Map (x * 2)\n");
        printf("8. Применить Where (только четные числа)\n");
        printf("9. Применить Reduce (сумма первых N элементов)\n");
        printf("10. Прочитать N элементов через SequenceStream\n");
        printf("11. Сбросить до базовой последовательности Фибоначчи\n");
        printf("12. Запустить тесты\n");
        printf("13. Записать через SequenceWriteStream\n");
        printf("0. Выход\n");

        choice = ReadInt("Выберите действие: ");

        try {
            switch (choice) {
                case 1: {
                    int count = ReadInt("Сколько элементов вывести? ");
                    if (count < 0) throw InvalidArgument("Количество не может быть отрицательным");
                    PrintSequenceElements(currentSeq, static_cast<size_t>(count));
                    break;
                }
                case 2: {
                    int index = ReadInt("Введите индекс: ");
                    if (index < 0) throw IndexOutOfRange("Индекс не может быть отрицательным");
                    printf("Элемент [%d] = %zu\n", index, (*currentSeq)[static_cast<size_t>(index)]);
                    break;
                }
                case 3: {
                    int val = ReadInt("Введите значение для Append: ");
                    currentSeq->Append(static_cast<size_t>(val));
                    printf("Правило Append успешно добавлено!\n");
                    break;
                }
                case 4: {
                    int val = ReadInt("Введите значение для Prepend: ");
                    currentSeq->Prepend(static_cast<size_t>(val));
                    printf("Правило Prepend успешно добавлено!\n");
                    break;
                }
                case 5: {
                    int idx = ReadInt("Введите индекс для вставки: ");
                    int val = ReadInt("Введите значение: ");
                    if (idx < 0) throw IndexOutOfRange("Индекс не может быть отрицательным");
                    currentSeq->InsertAt(static_cast<size_t>(val), static_cast<size_t>(idx));
                    printf("Правило InsertAt успешно добавлено!\n");
                    break;
                }
                case 6: {
                    currentSeq->RemoveLast();
                    printf("Правило RemoveLast успешно применено!\n");
                    break;
                }
                case 7: {
                    LazySequence<size_t>* mappedSeq = currentSeq->Map(&multiplier);
                    currentSeq = mappedSeq;
                    printf("Операция Map (умножение на 2) применена (создана новая ленивая обертка)!\n");
                    break;
                }
                case 8: {
                    LazySequence<size_t>* filteredSeq = currentSeq->Where(&evenFilter);
                    currentSeq = filteredSeq;
                    printf("Операция Where (четные числа) применена (создана новая ленивая обертка)!\n");
                    break;
                }
                case 9: {
                    int count = ReadInt("Сколько первых элементов просуммировать? ");
                    if (count < 0) throw InvalidArgument("Количество не может быть отрицательным");
                    size_t res = currentSeq->Reduce(&summer, 0, static_cast<size_t>(count));
                    printf("Результат Reduce (Сумма): %zu\n", res);
                    break;
                }
                case 10: {
                    int count = ReadInt("Сколько элементов прочитать из потока? ");
                    if (count < 0) throw InvalidArgument("Количество не может быть отрицательным");
                    
                    SequenceStream<size_t> stream(currentSeq);
                    stream.Open();
                    printf("Чтение из потока: [ ");
                    for (int i = 0; i < count; ++i) {
                        if (stream.IsEndOfStream()) {
                            printf("(конец потока) ");
                            break;
                        }
                        printf("%zu ", stream.Read());
                    }
                    printf("]\n");
                    stream.Close();
                    break;
                }
                case 11: {
                    delete currentSeq;
                    baseRule = new FibonacciRule();
                    currentSeq = new LazySequence<size_t>(baseRule);
                    printf("Сброс выполнен. Текущая последовательность — бесконечные числа Фибоначчи.\n");
                    break;
                }
                case 12: {
                    RunAllTests();
                    break;
                }
                case 13: {
                    int count = ReadInt("Сколько элементов записать через поток? ");
                    if (count < 0) throw InvalidArgument("Количество не может быть отрицательным.");
                    
                    SequenceWriteStream<size_t> writeStream(currentSeq);
                    writeStream.Open();
                    
                    for (int i = 0; i < count; ++i) {
                        printf("Введите элемент %d: ", i + 1);
                        int val = 0;
                        while (scanf("%d", &val) != 1) {
                            while (getchar() != '\n'); 
                            printf("Ошибка ввода. Введите число: ");
                        }
                        
                        writeStream.Write(static_cast<size_t>(val));
                    }
                    
                    writeStream.Close();
                    printf("Успешно записано элементов: %zu\n", writeStream.GetPosition());
                    break;
                }
                case 0:
                    printf("Завершение программы...\n");
                    break;
                default:
                    printf("Неизвестная команда.\n");
            }
        } catch (const Exception& e) {
            
            printf("\n[ОШИБКА]: %s\n", e.GetMessage());
        }
    }

    delete currentSeq;
    return 0;
}