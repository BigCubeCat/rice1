#include <atomic>
#include <cstdint>
#include <string>
#include <vector>

namespace crack_hash_worker::back {
class THashWorker {
public:
    THashWorker(
        const std::string &target_hash,
        int rank,
        int total_ranks,
        size_t max_size,
        std::string alphabet
    );

    void search();

    [[nodiscard]] bool isUpdated() const {
        return m_updated.load();
    }
    [[nodiscard]] bool isReady() const {
        return m_is_ready.load();
    }
    [[nodiscard]] const std::vector<std::string> &getResult() {
        m_updated.store(true);
        return m_result;
    }

private:
    std::string m_target_hash;
    int m_rank;
    int m_total_ranks;
    size_t m_max_size;
    std::string m_alphabet;
    std::vector<std::string> m_result;
    std::atomic<bool> m_is_ready = false;
    std::atomic<bool> m_updated  = false;

    /// Преобразование индекса в слово
    [[nodiscard]] std::string
    indexToWord(uint64_t index, size_t length, size_t base) const;

    static std::string toLower(std::string str);
};
};    // namespace crack_hash_worker::back
