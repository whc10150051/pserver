//
// Created by madrus on 26.06.16.
//

#ifndef PSERVER_FITTINGTASK_H
#define PSERVER_FITTINGTASK_H


#include "AbstractTask.h"

class FittingTask : public AbstractTask {
public:
    FittingTask(const Poco::JSON::Object::Ptr& config);

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


#endif //PSERVER_FITTINGTASK_H
