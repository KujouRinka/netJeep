#ifndef NETJEEP_CONFIG_H
#define NETJEEP_CONFIG_H

#include <fstream>
#include <vector>
#include <string>

#include "nlohmann/json.hpp"

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

Config *loadConfig(const char *filename);

void to_json(json &j, const Detail *detail);
void from_json(const json &j, Detail *&detail);
void to_json(json &j, const Accector &a);
void from_json(const json &j, Accector &a);
void to_json(json &j, const Dialer &d);
void from_json(const json &j, Dialer &d);
void to_json(json &j, const Config &c);
void from_json(const json &j, Config &c);

#endif