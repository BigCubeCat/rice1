#include "hashworker.hpp"
//
#include <algorithm>

#include <qdebug.h>

#include "md5.hpp"


crack_hash_worker::back::THashWorker::THashWorker(
    std::string id,
    const std::string &target_hash,
    int rank,
    int total_ranks,
    size_t max_size,
    std::string alphabet
)
    : m_id(std::move(id)),
      m_target_hash(toLower(target_hash)),
      m_rank(rank),
      m_total_ranks(total_ranks),
      m_max_size(max_size),
      m_alphabet(std::move(alphabet)) { }

void crack_hash_worker::back::THashWorker::search() {
    const size_t base = m_alphabet.size();

    for (size_t len = 1; len <= m_max_size; ++len) {
        uint64_t totalCombinations = 1;
        bool overflow              = false;
        for (size_t i = 0; i < len; ++i) {
            if (__builtin_mul_overflow(
                    totalCombinations, base, &totalCombinations
                )) {
                m_is_ready = true;
                return;
            }
        }
        uint64_t chunkSize  = totalCombinations / m_total_ranks;
        uint64_t startIndex = m_rank * chunkSize;
        uint64_t endIndex   = (m_rank == m_total_ranks - 1) ?
                                  totalCombinations :
                                  (m_rank + 1) * chunkSize;
        for (uint64_t idx = startIndex; idx < endIndex; ++idx) {
            std::string word = indexToWord(idx, len, base);
            const auto h     = crack_hash_worker::back::md5(word);
            if (h == m_target_hash) {
                m_result.push_back(word);
            }
        }
    }
    m_is_ready = true;
}

[[nodiscard]] std::string crack_hash_worker::back::THashWorker::indexToWord(
    uint64_t index, size_t length, size_t base
) const {
    std::string word(length, ' ');
    for (size_t i = 0; i < length; ++i) {
        word[length - 1 - i] = m_alphabet[index % base];
        index /= base;
    }
    return word;
}

std::string crack_hash_worker::back::THashWorker::toLower(std::string str) {
    std::transform(// NOLINT
        str.begin(), str.end(), str.begin(), [](unsigned char c) {
            return std::tolower(c);
        }
    );
    return str;
}
