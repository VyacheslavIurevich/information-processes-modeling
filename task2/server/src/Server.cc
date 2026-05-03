#include "Server.h"
#include "RepairMessage_m.h"

Define_Module(Server);

void Server::initialize() {
    serverId = getIndex();
    attackTimer = new cMessage("attackTimer");
    
    // Запускаем генерацию первой атаки
    double mean = par("attackMean").doubleValue();
    scheduleAt(simTime() + exponential(mean * 3600), attackTimer);
}

void Server::handleMessage(cMessage *msg) {
    if (msg == attackTimer) {
        // Планируем следующую атаку (экспоненциальное распределение)
        double mean = par("attackMean").doubleValue();
        scheduleAt(simTime() + exponential(mean * 3600), attackTimer);

        if (isWorking) {
            double p = uniform(0, 1);
            if (p >= 0.5) { 
                // 50% атак отражается. Остальные 50% ломают сервер.
                // Из этих 50%, 20% (т.е. 0.4 от успешных) - частичный выход, 30% (0.6 от успешных) - полный.
                isWorking = false;
                RepairRequest *req = new RepairRequest("repairReq");
                req->setServerId(serverId);
                
                if (uniform(0, 1) < 0.4) {
                    // Частичный выход из строя: 8 ± 4 часа
                    req->setRepairDuration(uniform(4, 12) * 3600); 
                } else {
                    // Полный выход из строя: 16 ± 8 часов
                    req->setRepairDuration(uniform(8, 24) * 3600);
                }
                send(req, "out");
            }
        }
    } else {
        // Получен ответ от программиста: сервер восстановлен
        isWorking = true;
        delete msg;
    }
}
