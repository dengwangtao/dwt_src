#include "Config.h"

#include "json.hpp"
#include "tcp/Logger.h"

#include <fstream>

namespace dwt {



using json = nlohmann::json;

void Config::loadConf(const std::string& file_path) {
    json config;
    std::ifstream f(file_path);
    if(f.fail()) {
        LOG_FATAL("%s configuration file open error", file_path.c_str());
    }

    json data = json::parse(f);

    for(json::iterator it = data.begin(); it != data.end(); ++ it) {
        m_conf[it.key()] = it.value();
        LOG_INFO("load config %s=%s", it.key().c_str(), m_conf[it.key()].c_str());
    }
}

std::string Config::Get(const std::string& key) const {
    auto f = m_conf.find(key);
    if(f != m_conf.end()) {
        return f->second;
    }
    LOG_FATAL("config key: %s not found", key.c_str());
    return {};
}

} // end namespace dwt