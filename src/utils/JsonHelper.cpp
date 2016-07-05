//
// Created by madrus on 26.06.16.
//

#include "JsonHelper.h"

int JsonHelper::getIntProperty(Poco::JSON::Object::Ptr objectPtr, const std::string& propertyName) {
    return getProperty<int>(objectPtr, propertyName, [](Poco::Dynamic::Var propertyVar) {
        return propertyVar.isInteger();
    });
}

std::string JsonHelper::getStringProperty(Poco::JSON::Object::Ptr objectPtr, const std::string& propertyName) {
    return getProperty<std::string>(objectPtr, propertyName, [](Poco::Dynamic::Var propertyVar) {
        return propertyVar.isString();
    });
}

Poco::JSON::Object::Ptr JsonHelper::getObjectProperty(Poco::JSON::Object::Ptr objectPtr, const std::string& propertyName) {
    return getProperty<Poco::JSON::Object::Ptr>(objectPtr, propertyName, [](Poco::Dynamic::Var propertyVar) {
        return true;
    }, [](Poco::Dynamic::Var propertyVar) {
        return propertyVar.extract<Poco::JSON::Object::Ptr>();
    });
}

Poco::JSON::Array::Ptr JsonHelper::getArrayProperty(Poco::JSON::Object::Ptr objectPtr, const std::string& propertyName) {
    return getProperty<Poco::JSON::Array::Ptr>(objectPtr, propertyName, [](Poco::Dynamic::Var propertyVar) {
        //return propertyVar.isArray();
        return true;
    }, [](Poco::Dynamic::Var propertyVar) {
        return propertyVar.extract<Poco::JSON::Array::Ptr>();
    });
}

bool JsonHelper::hasProperty(Poco::JSON::Object::Ptr objectPtr, const std::string& propertyName) {
    return objectPtr->has(propertyName);
}
