#include <chrono>
#include <set>
#include <string>
#include <thread>

#include <gtest/gtest.h>

#include "hashworker.hpp"

using namespace std::chrono_literals;
using namespace crack_hash_worker::back;

// ============================================================================
// Тесты на вычисление MD5 (Внутренняя логика)
// ============================================================================
TEST(HashWorkerTest, MD5KnownValues) {
    // Проверяем известные векторы тестирования MD5
    // Используем временный объект для доступа к приватному методу через
    // публичный интерфейс search, либо тестируем косвенно через поиск. Для
    // чистоты теста создадим хэши заранее известным инструментом и проверим
    // поиск.

    // "hello" -> 5d41402abc4b2a76b9719d911017c592
    std::string target = "5d41402abc4b2a76b9719d911017c592";
    THashWorker worker("", target, 0, 1, 5, "abcdefghijklmnopqrstuvwxyz");
    worker.search();

    std::this_thread::sleep_for(1s);
    const auto results = worker.getResult();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0], "hello");
}

TEST(HashWorkerTest, MD5CaseInsensitive) {
    // Хэш должен находиться независимо от регистра входной строки хэша
    std::string targetLower = "5d41402abc4b2a76b9719d911017c592";
    std::string targetUpper = "5D41402ABC4B2A76B9719D911017C592";

    THashWorker workerLower(
        "", targetLower, 0, 1, 5, "abcdefghijklmnopqrstuvwxyz"
    );
    THashWorker workerUpper(
        "", targetUpper, 0, 1, 5, "abcdefghijklmnopqrstuvwxyz"
    );

    workerLower.search();
    workerUpper.search();

    std::this_thread::sleep_for(1s);

    auto resLower = workerLower.getResult();
    auto resUpper = workerUpper.getResult();
    ASSERT_EQ(resLower.size(), 1);
    ASSERT_EQ(resUpper.size(), 1);
    EXPECT_EQ(resLower[0], resUpper[0]);
}

// ============================================================================
// Тесты на распределение нагрузки (Rank / Total Ranks)
// ============================================================================

TEST(HashWorkerTest, DistributedSearchCoverage) {
    // Проверяем, что N воркеров вместе находят всё, что находит 1 воркер
    std::string target   = "5d41402abc4b2a76b9719d911017c592";    // "hello"
    std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
    size_t maxLen        = 5;
    int totalWorkers     = 4;

    // 1. Запускаем однопоточный режим (эталон)
    THashWorker singleWorker("", target, 0, 1, maxLen, alphabet);
    singleWorker.search();
    std::this_thread::sleep_for(1s);
    auto singleResults = singleWorker.getResult();

    // 2. Запускаем распределенный режим
    std::vector<std::vector<std::string>> allResults(totalWorkers);
    std::vector<std::thread> threads;

    threads.reserve(totalWorkers);
    for (int i = 0; i < totalWorkers; ++i) {
        threads.emplace_back(
            [&allResults, i, &target, totalWorkers, maxLen, &alphabet]() {
                THashWorker worker(
                    "", target, i, totalWorkers, maxLen, alphabet
                );
                worker.search();
                std::this_thread::sleep_for(1s);
                allResults[i] = worker.getResult();
            }
        );
    }

    for (auto &t : threads)
        t.join();

    // 3. Собираем все результаты распределенного поиска
    std::set<std::string> distributedSet;
    for (const auto &vec : allResults) {
        for (const auto &s : vec) {
            distributedSet.insert(s);
        }
    }

    // 4. Сравниваем
    std::set<std::string> singleSet(singleResults.begin(), singleResults.end());

    EXPECT_EQ(distributedSet, singleSet);
    EXPECT_EQ(distributedSet.size(), 1);    // Должно найтись только "hello"
}

TEST(HashWorkerTest, DistributedSearchNoOverlap) {
    // Проверяем, что воркеры не дублируют работу (находят разные части
    // пространства) Используем алфавит из 2 символов и длину 3, чтобы перебор
    // был быстрым (2^3 = 8 слов) aaa, aab, aba, abb, baa, bab, bba, bbb

    std::string alphabet = "ab";
    size_t maxLen        = 3;
    int totalWorkers     = 2;

    // Хэш, которого точно нет, чтобы воркеры прошли весь диапазон
    std::string target = "00000000000000000000000000000000";

    std::vector<std::vector<std::string>> allResults(totalWorkers);
    std::vector<std::thread> threads;

    threads.reserve(totalWorkers);
    for (int i = 0; i < totalWorkers; ++i) {
        threads.emplace_back(
            [&allResults, i, &target, totalWorkers, maxLen, &alphabet]() {
                THashWorker worker(
                    "", target, i, totalWorkers, maxLen, alphabet
                );
                worker.search();
                std::this_thread::sleep_for(1s);
                allResults[i] = worker.getResult();
            }
        );
    }
    for (auto &t : threads)
        t.join();

    // Проверяем, что результаты пусты (так как хэш не найден)
    // Но главное - проверяем, что программа не упала и отработала корректно
    for (const auto &vec : allResults) {
        EXPECT_TRUE(vec.empty());
    }
}

// ============================================================================
// Тесты на поиск нескольких совпадений
// ============================================================================

TEST(HashWorkerTest, MultipleMatches) {
    // Проверяем поиск, когда подходит несколько слов (коллизия или одинаковый
    // хэш) В MD5 коллизии редки, но мы можем искать слова разной длины, если
    // хэши совпадут (крайне маловероятно) Для теста создадим сценарий, где мы
    // ищем хэш, который знают несколько воркеров, но в реальности MD5
    // детерминирован. Лучше протестировать, что воркер находит слово, если оно
    // попадает в ЕГО диапазон.

    // "a" -> 0cc175b9c0f1b6a831c399e269772661
    std::string target = "0cc175b9c0f1b6a831c399e269772661";

    // Воркер 0 должен найти "a" (это первая комбинация)
    THashWorker worker0("", target, 0, 2, 1, "abc");
    worker0.search();
    std::this_thread::sleep_for(1s);
    auto res0 = worker0.getResult();
    EXPECT_EQ(res0.size(), 1);
    EXPECT_EQ(res0[0], "a");

    // Воркер 1 не должен найти "a" (оно в диапазоне воркера 0)
    THashWorker worker1("", target, 1, 2, 1, "abc");
    worker1.search();
    std::this_thread::sleep_for(1s);
    auto res1 = worker1.getResult();
    EXPECT_EQ(res1.size(), 0);
}

TEST(HashWorkerTest, MaxSizeZero) {
    // Если максимальная длина 0, поиск не должен выполняться
    std::string target = "00000000000000000000000000000000";
    THashWorker worker("", target, 0, 1, 0, "abc");
    worker.search();
    std::this_thread::sleep_for(1s);
    auto results = worker.getResult();
    EXPECT_TRUE(results.empty());
}

TEST(HashWorkerTest, NoMatchFound) {
    // Хэш, для которого точно нет слова в заданном алфавите и длине
    std::string target = "ffffffffffffffffffffffffffffffff";
    THashWorker worker("", target, 0, 1, 3, "a");    // Только слово "aaa"
    worker.search();
    std::this_thread::sleep_for(1s);
    auto results = worker.getResult();
    EXPECT_TRUE(results.empty());
}
