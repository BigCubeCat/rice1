#include "utils.hpp"
//
#include <iomanip>
#include <memory>
#include <random>
#include <sstream>
#include <string>

#include "request.hxx"


QString utils::task2body(const Task &task, int size, int rank) {
    xercesc::XMLPlatformUtils::Initialize();
    static auto makeAlphabet = []() {
        auto alphabet = std::make_unique<dto::Alphabet>();
        for (int i = 97; i < 123; ++i) {
            std::array<char, 1> a = { static_cast<char>(i) };
            alphabet->symbols().push_back(a.data());
        }
        return std::move(alphabet);
    };
    xercesc::XMLPlatformUtils::Initialize();
    auto alphabet = makeAlphabet();
    dto::CrackHashManagerRequest req(
        task.requestId.toUtf8().data(),
        rank,
        size,
        task.hash.toUtf8().data(),
        task.maxLength,
        std::move(alphabet)
    );
    std::ostringstream oss;
    dto::CrackHashManagerRequest_(oss, req);
    xercesc::XMLPlatformUtils::Terminate();
    return QString::fromStdString(oss.str());
}

QStringList utils::task2bodys(const Task &task, int size) {
    QStringList bodys;
    bodys.reserve(size);
    for (int i = 0; i < size; i++) {
        bodys << task2body(task, size, i);
    }
    return bodys;
}

QString utils::generateUUID() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::string hexChars = "0123456789abcdef";

    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < 8; i++) {
        ss << hexChars[dis(gen)];
    }
    ss << "-";
    for (int i = 0; i < 4; i++) {
        ss << hexChars[dis(gen)];
    }
    ss << "-";
    for (int i = 0; i < 4; i++) {
        ss << hexChars[dis(gen)];
    }
    ss << "-";
    for (int i = 0; i < 4; i++) {
        ss << hexChars[dis(gen)];
    }
    ss << "-";
    for (int i = 0; i < 12; i++) {
        ss << hexChars[dis(gen)];
    }
    return QString::fromStdString(ss.str());
}
