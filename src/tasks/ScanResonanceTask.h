//
// Created by madrus on 10.07.16.
//

#pragma once

#include "AbstractTask.h"
#include <Poco/JSON/Object.h>

/**
 * реализует почастотное зондирование и поиск резонанса
 */
class ScanResonanceTask : public AbstractTask {
public:
    ScanResonanceTask(const Poco::JSON::Object::Ptr& config);

    virtual bool run() override;

private:
    bool _sensIsHi = false;
    bool _voltIsHi = false;
    int _channel = 0;
    int _relay = 0;
    double _tau = 0;
    int _l = 0;
    int _m = 10;
    int _nStart = 0;
    int _lPost = 0;
    int _size = 0;
};
