#include "ServiceName.h"
#include <errno.h>

std::string getServiceName(unsigned long int pid) {
    return program_invocation_name;
}