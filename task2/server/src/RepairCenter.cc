#include "RepairCenter.h"
#include "RepairMessage_m.h"

Define_Module(RepairCenter);

void RepairCenter::initialize() {
    numProgrammers = par("numProgrammers");
    busyProgrammers = 0;
    currentFailedServers = 0;
    failedServersSignal = registerSignal("failedServers");
    emit(failedServersSignal, currentFailedServers);
}

void RepairCenter::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        // Ремонт завершен
        busyProgrammers--;
        
        // Достаем исходный запрос из контекста сообщения
        RepairRequest *req = (RepairRequest *)msg->getContextPointer();
        
        // Возвращаем ответ серверу, что он починен
        cMessage *response = new cMessage("repairResp");
        send(response, "out", req->getServerId());
        
        currentFailedServers--;
        emit(failedServersSignal, currentFailedServers);
        
        delete req;
        delete msg;

        // Если в очереди есть ожидающие сервера, берем следующий
        if (!queue.isEmpty()) {
            RepairRequest *nextReq = (RepairRequest *)queue.pop();
            startRepair(nextReq);
        }
    } else {
        // Поступил новый запрос на ремонт (сервер сломался)
        currentFailedServers++;
        emit(failedServersSignal, currentFailedServers);
        
        if (busyProgrammers < numProgrammers) {
            startRepair(msg);
        } else {
            queue.insert(msg); // Свободных программистов нет, ставим в очередь
        }
    }
}

void RepairCenter::startRepair(cMessage *reqMsg) {
    busyProgrammers++;
    RepairRequest *req = check_and_cast<RepairRequest*>(reqMsg);
    
    // Создаем событие окончания ремонта
    cMessage *doneMsg = new cMessage("repairDone");
    doneMsg->setContextPointer(req); // Сохраняем запрос, чтобы знать кого будить
    scheduleAt(simTime() + req->getRepairDuration(), doneMsg);
}
