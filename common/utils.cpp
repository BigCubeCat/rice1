#include "utils.hpp"
//
#include <iomanip>
#include <memory>
#include <random>
#include <sstream>
#include <string>

#include "request.hxx"
#include "response.hxx"


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

TaskPart utils::body2taskPart(const QString &body) {
    xercesc::XMLPlatformUtils::Initialize();
    std::istringstream iss(body.toStdString());
    std::unique_ptr<dto::CrackHashWorkerResponse> res(
        dto::CrackHashWorkerResponse__(iss, xml_schema::flags::dont_validate)
    );
    TaskPart tp;
    tp.partNumber    = res->PartNumber();
    const auto words = res->Answers().words();
    tp.answers       = {};
    for (const auto &word : words) {
        tp.answers.push_back(word);
    }
    xercesc::XMLPlatformUtils::Terminate();
    return tp;
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

std::optional<QString> utils::getEnv(const std::string &name) {
    const char *val = std::getenv(name.c_str());
    return val ? QString(val) : std::nullopt;
}
