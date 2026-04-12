#include "processor.hpp"
//
#include <QString>
#include <memory>
#include <sstream>
#include <string>

#include "request.hxx"
#include "response.hxx"


TProcessor::TProcessor(QString url, QObject *parent)
    : QObject(parent),
      m_managerUrl(std::move(url)),
      m_worker(new TNetworkWorker(this)) {
    QObject::connect(&m_timer, &QTimer::timeout, this, &TProcessor::process);
    m_timer.setInterval(100);
    m_timer.start();
}

void TProcessor::addTask(const crack_hash_worker::back::THashWorker &worker) {
    m_queue.push(worker);
}

void TProcessor::process() {
    if (m_current) {
        auto result  = m_current->getResult();
        auto isReady = m_current->isReady();
        if (isReady) {
            m_queue.pop();
            // TODO: send request
            m_worker->sendPatchRequest(m_managerUrl, "");
        }
    }
    if (m_queue.empty()) {
        m_current = nullptr;
        return;
    }
    m_current = &m_queue.front();
    m_current->search();
}

crack_hash_worker::back::THashWorker utils::body2worker(const QString &body) {
    xercesc::XMLPlatformUtils::Initialize();
    std::istringstream iss(body.toStdString());
    std::unique_ptr<dto::CrackHashManagerRequest> req(
        dto::CrackHashManagerRequest_(iss, xml_schema::flags::dont_validate)
    );

    std::string alphabetString;
    auto symbols = req->Alphabet().symbols();
    alphabetString.reserve(symbols.size());
    for (const auto &s : symbols) {
        alphabetString += s;
    }
    crack_hash_worker::back::THashWorker worker(
        req->RequestId(),
        req->PartNumber(),
        req->PartCount(),
        req->MaxLength(),
        alphabetString
    );

    xercesc::XMLPlatformUtils::Terminate();

    return worker;
}
