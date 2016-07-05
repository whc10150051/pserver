//
// Created by madrus on 26.06.16.
//

#ifndef PSERVER_JSONHELPER_H
#define PSERVER_JSONHELPER_H


#include <string>
#include <Poco/JSON/Array.h>
#include <Poco/JSON/Object.h>

/**
 * Utilities to deal with JSON configuration
 */
class JsonHelper {
public:
    static int getIntProperty(Poco::JSON::Object::Ptr objectPtr, const std::string& propertyName);
    static std::string getStringProperty(Poco::JSON::Object::Ptr objectPtr, const std::string& propertyName);
    static Poco::JSON::Object::Ptr getObjectProperty(Poco::JSON::Object::Ptr objectPtr, const std::string& propertyName);
    static Poco::JSON::Array::Ptr getArrayProperty(Poco::JSON::Object::Ptr objectPtr, const std::string& propertyName);
    static bool hasProperty(Poco::JSON::Object::Ptr objectPtr, const std::string& propertyName);

private:
    template <typename T>
    class DefaultConvertFunc {
    public:
        T operator()(Poco::Dynamic::Var propertyVar) {
            return propertyVar.convert<T>();
        }
    };

    template <typename T, typename CheckTypeFunc>
    static T getProperty(Poco::JSON::Object::Ptr objectPtr, const std::string& propertyName,
                         CheckTypeFunc checkTypeFunc) {
        return getProperty<T>(objectPtr, propertyName, checkTypeFunc, DefaultConvertFunc<T>());
    }

    template <typename T, typename CheckTypeFunc, typename ConvertFunc = DefaultConvertFunc<T>>
    static T getProperty(Poco::JSON::Object::Ptr objectPtr, const std::string& propertyName,
                         CheckTypeFunc checkTypeFunc, ConvertFunc convertFunc) {
        if (!objectPtr->has(propertyName)) {
            throw Poco::Exception(Poco::format("Property %s not found", propertyName));
        }

        Poco::Dynamic::Var propertyVar;
        try {
            propertyVar = objectPtr->get(propertyName);
        }
        catch (const Poco::Exception& ex) {
            throw Poco::Exception(Poco::format("Failed to read property %s", propertyName), ex);
        }

        if (!checkTypeFunc(propertyVar)) {
            throw Poco::Exception(Poco::format("Failed to read property %s: unexpected type", propertyName));
        }

        try {
            T result = convertFunc(propertyVar);
            return result;
        }
        catch (const Poco::Exception& ex) {
            throw Poco::Exception(Poco::format("Failed to convert property %s", propertyName), ex);
        }
    }
};

#endif //PSERVER_C1_H
