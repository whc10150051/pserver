//
// Created by madrus on 26.06.16.
//

#pragma once

#include <Poco/Logger.h>

// logger macros that should be used in classes defined _logger member variable

#define LOG_CRITICAL(...) _logger.critical(__VA_ARGS__)
#define LOG_DEBUG(...) _logger.debug(__VA_ARGS__)
#define LOG_ERROR(...) _logger.error(__VA_ARGS__)
#define LOG_FATAL(...) _logger.fatal(__VA_ARGS__)
#define LOG_INFO(...) _logger.information(__VA_ARGS__)
#define LOG_NOTICE(...) _logger.notice(__VA_ARGS__)
#define LOG_TRACE(...) _logger.trace(__VA_ARGS__)
#define LOG_WARNING(...) _logger.warning(__VA_ARGS__)

// logger macros that may be used anywhere, global logger will be used

#define LOG_CRITICAL_A(...) Poco::Util::Application::instance().logger().critical(__VA_ARGS__)
#define LOG_DEBUG_A(...) Poco::Util::Application::instance().logger().debug(__VA_ARGS__)
#define LOG_ERROR_A(...) Poco::Util::Application::instance().logger().error(__VA_ARGS__)
#define LOG_FATAL_A(...) Poco::Util::Application::instance().logger().fatal(__VA_ARGS__)
#define LOG_INFO_A(...) Poco::Util::Application::instance().logger().information(__VA_ARGS__)
#define LOG_NOTICE_A(...) Poco::Util::Application::instance().logger().notice(__VA_ARGS__)
#define LOG_TRACE_A(...) Poco::Util::Application::instance().logger().trace(__VA_ARGS__)
#define LOG_WARNING_A(...) Poco::Util::Application::instance().logger().warning(__VA_ARGS__)
