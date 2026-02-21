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

    [[nodiscard]] std::vector<std::string> search() const;

private:
    std::string m_target_hash;
    int m_rank;
    int m_total_ranks;
    size_t m_max_size;
    std::string m_alphabet;

    /// Преобразование индекса в слово
    [[nodiscard]] std::string
    indexToWord(uint64_t index, size_t length, size_t base) const;

    static std::string toLower(std::string str);
};
};    // namespace crack_hash_worker::back
