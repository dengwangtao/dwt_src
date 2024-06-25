#pragma once

#include <unordered_map>
#include <string>

namespace dwt {


class Config {
    
public:
    void loadConf(const std::string& file_path);

    std::string Get(const std::string& key) const;

private:
    std::unordered_map<std::string, std::string> m_conf;
};


} // end namespace dwt