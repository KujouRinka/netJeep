#ifndef NETJEEP_CONFIG_H
#define NETJEEP_CONFIG_H

#include <fstream>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

using namespace std;
using namespace nlohmann;

struct Detail {
    string address;
    string type;
    string password;
    uint16_t port;
};

struct Accector {
    string tag;
    string protocol;
    Detail *detail;
};

struct Dialer {
    string tag;
    string protocol;
    Detail *detail;
};

struct Config {
    vector<Accector> acceptors;
    vector<Dialer> dialers;
};

void to_json(json &j, const Detail *detail) {
    if (detail == nullptr) {
        j = nullptr;
        return;
    }
    j = json{
        {"address",  detail->address},
        {"type",     detail->type},
        {"password", detail->password},
        {"port",     detail->port}
    };
}

void from_json(const json &j, Detail *&detail) {
    if (detail == nullptr)
        detail = new Detail();
    try {
        j.at("address").get_to(detail->address);
    } catch (...) {}
    try {
        j.at("type").get_to(detail->type);
    }
    catch (...) {
        detail->type = "domain";
    }
    try {
        j.at("password").get_to(detail->password);
    } catch (...) {}
    try {
        j.at("port").get_to(detail->port);
    } catch (...) {}
}

void to_json(json &j, const Accector &a) {
    j = json{
        {"tag",      a.tag},
        {"protocol", a.protocol},
        {"detail",   a.detail},
    };
}

void from_json(const json &j, Accector &a) {
    try {
        j.at("tag").get_to(a.tag);
    } catch (...) {}
    try {
        j.at("protocol").get_to(a.protocol);
    } catch (...) {}
    try {
        j.at("detail").get_to(a.detail);
    } catch (json::out_of_range &e) {
        a.detail = nullptr;
    }
}

void to_json(json &j, const Dialer &d) {
    j = json{
        {"tag",      d.tag},
        {"protocol", d.protocol},
        {"detail",   d.detail},
    };
}

void from_json(const json &j, Dialer &d) {
    try {
        j.at("tag").get_to(d.tag);
    } catch (...) {}
    try {
        j.at("protocol").get_to(d.protocol);
    } catch (...) {}
    try {
        j.at("detail").get_to(d.detail);
    } catch (json::out_of_range &e) {
        d.detail = nullptr;
    }
}

void to_json(json &j, const Config &c) {
    j = json({{"acceptors", c.acceptors},
              {"dialers",   c.dialers}});
}

void from_json(const json &j, Config &c) {
    try {
        j.at("acceptors").get_to(c.acceptors);
    } catch (...) {}
    try {
        j.at("dialers").get_to(c.dialers);
    } catch (...) {}
}

Config *loadConfig(const char *filename) {
    ifstream f(filename);
    json j;
    f >> j;
    f.close();
    auto config = new Config;
    *config = j.get<Config>();
    return config;
}

#endif