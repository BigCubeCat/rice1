#include "processor.hpp"
//
#include <QString>
#include <fstream>
#include <memory>
#include <sstream>

#include "request.hxx"


TProcessor::TProcessor(QObject *parent) : QObject(parent) { }

void TProcessor::addTask(const crack_hash_worker::back::THashWorker &worker) {
    m_queue.push(worker);
}

crack_hash_worker::back::THashWorker utils::body2worker(const QString &body) {
    xercesc::XMLPlatformUtils::Initialize();
    std::istringstream iss(body.toStdString());
    std::unique_ptr<dto::CrackHashManagerRequest> req(
        dto::CrackHashManagerRequest_(iss, xml_schema::flags::dont_validate)
    );

    std::string alphabet;
    const auto &symbols = req->Alphabet().symbols();
    alphabet.reserve(symbols.size());
    for (const auto &s : symbols) {
        alphabet += s;
    }
    crack_hash_worker::back::THashWorker worker(
        req->RequestId(),
        req->PartNumber(),
        req->PartCount(),
        req->MaxLength(),
        alphabet
    );

    xercesc::XMLPlatformUtils::Terminate();

    return worker;
}
