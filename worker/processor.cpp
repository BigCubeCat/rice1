#include "processor.hpp"
//
#include <QString>
#include <memory>
#include <sstream>
#include <string>

#include <qlogging.h>

#include "request.hxx"
#include "response.hxx"
#include "task.hpp"
#include "utils.hpp"


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
            TaskPart tp;
            tp.partNumber = m_current->rank();
            tp.answers.reserve(static_cast<int>(result.size()));
            for (const auto &word : result) {
                tp.answers.push_back(QString::fromStdString(word));
            }
            const auto body = utils::taskPart2body(tp, m_current->id());
            m_worker->sendPatchRequest(
                m_managerUrl + "/internal/api/manager/hash/crack/request", body
            );
            
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
        req->Hash(),
        req->PartNumber(),
        req->PartCount(),
        req->MaxLength()
    );

    xercesc::XMLPlatformUtils::Terminate();

    return worker;
}
