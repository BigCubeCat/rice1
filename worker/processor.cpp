#include "processor.hpp"
//
#include <QString>
#include <fstream>
#include <memory>
#include <sstream>

#include "request.hxx"


TProcessor::TProcessor(QObject *parent) : QObject(parent) {
    QObject::connect(&m_timer, &QTimer::timeout, this, &TProcessor::process);
    m_timer.setInterval(100);
    m_timer.start();
}

void TProcessor::addTask(const crack_hash_worker::back::THashWorker &worker) {
    m_queue.push(worker);
}

void TProcessor::process() {
    if (m_queue.empty()) {
        return;
    }
    auto worker = std::move(m_queue.front());
    m_queue.pop();
    worker.search();
    if (worker.isUpdated()) {
        const auto &result = worker.getResult();
    }
}

TProcessor::~TProcessor() { }


crack_hash_worker::back::THashWorker utils::body2worker(const QString &body) {
    xercesc::XMLPlatformUtils::Initialize();
    std::istringstream iss(body.toStdString());
    std::unique_ptr<dto::CrackHashManagerRequest> req(
        dto::CrackHashManagerRequest_(iss, xml_schema::flags::dont_validate)
    );

    std::string alphabet;
    const auto &symbols = req->alphabet().symbols();
    alphabet.reserve(symbols.size());
    for (const auto &s : symbols) {
        alphabet += s;
    }
    THashWorker worker(
        req->requestId(),
        req->partNumber(),
        req->partCount(),
        req->maxLength(),
        alphabet
    );

    xercesc::XMLPlatformUtils::Terminate();

    return worker;
}
