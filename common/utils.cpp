#include "utils.hpp"
//
#include <QString>
#include <iomanip>
#include <memory>
#include <optional>
#include <random>
#include <sstream>
#include <string>

#include <qdebug.h>

#include "request.hxx"
#include "response.hxx"

static dto::Answers::words_sequence
toWordsSequence(const QVector<QString> &qtWords) {
    dto::Answers::words_sequence result;
    for (const QString &word : qtWords) {
        // QString → std::string → xml_schema::string
        result.push_back(word.toStdString());
    }
    return result;
}


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
        dto::CrackHashWorkerResponse_(iss, xml_schema::flags::dont_validate)
    );
    TaskPart tp;
    tp.partNumber    = res->PartNumber();
    const auto words = res->Answers().words();
    tp.answers       = {};
    for (const auto &word : words) {
        tp.answers.push_back(word.data());
    }
    xercesc::XMLPlatformUtils::Terminate();
    return tp;
}

QString utils::taskPart2body(const TaskPart &tp, const std::string &id) {
    dto::Answers answers;
    answers.words() = toWordsSequence(tp.answers);
    QString rid     = QString::fromStdString(id);
    dto::CrackHashWorkerResponse response("", tp.partNumber, answers);

    std::ostringstream oss;
    dto::CrackHashWorkerResponse_(oss, response);
    xercesc::XMLPlatformUtils::Terminate();
    return QString::fromStdString(oss.str());
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
    if (!val)
        return std::nullopt;

    return QString::fromUtf8(val);
}
