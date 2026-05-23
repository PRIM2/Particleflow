// ConfigParser.cc
#include "config/ConfigParser.hh"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

namespace config {

namespace {

enum class ValueType {
    String,
    Int,
    Double,
    Bool
};

struct ConfigKey {
    std::string section;
    std::string key;
    ValueType type;
};

const std::vector<ConfigKey> allowedKeys = {
    {"Run",    "nEvents",    ValueType::Int},
    {"Run",    "mode",       ValueType::String},
    {"Run",    "hepmcOutput", ValueType::String},

    {"Pythia", "cmndFile",   ValueType::String},

    {"Pgun",   "energy",     ValueType::Double},
    {"Pgun",   "pdg",        ValueType::Int},
    {"Pgun",   "mass",       ValueType::Double},

    {"Geant4", "input",      ValueType::String},
    {"Geant4", "output",     ValueType::String},
    {"Geant4", "ui",         ValueType::Bool},
    {"Geant4", "geometry",   ValueType::String},

    {"Geant4.AngularFilter", "enabled", ValueType::Bool},
    {"Geant4.AngularFilter", "targetX", ValueType::Double},
    {"Geant4.AngularFilter", "targetY", ValueType::Double},
    {"Geant4.AngularFilter", "targetZ", ValueType::Double},
    {"Geant4.AngularFilter", "theta",   ValueType::Double},

    {"Geant4.Geometry.RealisticMap", "concreteHalfX", ValueType::Double},
    {"Geant4.Geometry.RealisticMap", "frontPbHalfZ",  ValueType::Double},
};


std::string trim(const std::string& text) {
    const auto begin = std::find_if_not(
        text.begin(),
        text.end(),
        [](unsigned char c) {
            return std::isspace(c);
        }
    );

    if (begin == text.end()) {
        return "";
    }

    const auto end = std::find_if_not(
        text.rbegin(),
        text.rend(),
        [](unsigned char c) {
            return std::isspace(c);
        }
    ).base();

    return std::string(begin, end);
}

std::string toLower(std::string value) {
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        }
    );

    return value;
}

std::string makeKey(const std::string& section, const std::string& key) {
    return section + "." + key;
}

std::string removeInlineComment(const std::string& line) {
    bool insideQuotes = false;

    for (std::size_t i = 0; i < line.size(); i++) {
        if (line[i] == '"') {
            insideQuotes = !insideQuotes;
        }

        if (line[i] == '#' && !insideQuotes) {
            return line.substr(0, i);
        }
    }

    return line;
}

bool isQuotedString(const std::string& value) {
    return value.size() >= 2 && value.front() == '"' && value.back() == '"';
}

std::string parseString(const std::string& rawValue, const std::string& fullKey) {
    if (!isQuotedString(rawValue)) {
        throw std::runtime_error(
            "Invalid type for " + fullKey +
            ": expected string between quotes, got: " + rawValue
        );
    }

    return rawValue.substr(1, rawValue.size() - 2);
}

int parseInt(const std::string& rawValue, const std::string& fullKey) {
    if (isQuotedString(rawValue)) {
        throw std::runtime_error(
            "Invalid type for " + fullKey +
            ": expected int without quotes, got: " + rawValue
        );
    }

    try {
        std::size_t pos = 0;
        int value = std::stoi(rawValue, &pos);

        if (pos != rawValue.size()) {
            throw std::invalid_argument("trailing characters");
        }

        return value;
    } catch (const std::exception&) {
        throw std::runtime_error(
            "Invalid type for " + fullKey +
            ": expected int, got: " + rawValue
        );
    }
}

double parseDouble(const std::string& rawValue, const std::string& fullKey) {
    if (isQuotedString(rawValue)) {
        throw std::runtime_error(
            "Invalid type for " + fullKey +
            ": expected double without quotes, got: " + rawValue
        );
    }

    try {
        std::size_t pos = 0;
        double value = std::stod(rawValue, &pos);

        if (pos != rawValue.size()) {
            throw std::invalid_argument("trailing characters");
        }

        return value;
    } catch (const std::exception&) {
        throw std::runtime_error(
            "Invalid type for " + fullKey +
            ": expected double, got: " + rawValue
        );
    }
}

bool parseBool(const std::string& rawValue, const std::string& fullKey) {
    if (isQuotedString(rawValue)) {
        throw std::runtime_error(
            "Invalid type for " + fullKey +
            ": expected bool without quotes, got: " + rawValue
        );
    }

    const std::string value = toLower(rawValue);

    if (value == "true") {
        return true;
    }

    if (value == "false") {
        return false;
    }

    throw std::runtime_error(
        "Invalid type for " + fullKey +
        ": expected true or false, got: " + rawValue
    );
}

const ConfigKey* findAllowedKey(
    const std::string& section,
    const std::string& key
) {
    for (const auto& allowed : allowedKeys) {
        if (allowed.section == section && allowed.key == key) {
            return &allowed;
        }
    }

    return nullptr;
}

void assignValue(
    SimulationConfig& cfg,
    const std::string& section,
    const std::string& key,
    const std::string& rawValue,
    ValueType type
) {
    const std::string fullKey = makeKey(section, key);

    switch (type) {
        case ValueType::String: {
            const std::string value = parseString(rawValue, fullKey);

            if (section == "Run" && key == "mode") {
                cfg.run.mode = value;
                return;
            }

            if (section == "Run" && key == "hepmcOutput") {
                cfg.run.hepmcOutput = value;
                return;
            }

            if (section == "Pythia" && key == "cmndFile") {
                cfg.pythia.cmndFile = value;
                return;
            }

            if (section == "Geant4" && key == "input") {
                cfg.geant4.input = value;
                return;
            }

            if (section == "Geant4" && key == "output") {
                cfg.geant4.output = value;
                return;
            }

            if (section == "Geant4" && key == "geometry") {
                cfg.geant4.geometry = value;
                return;
            }

            break;
        }
        case ValueType::Int: {
            const int value = parseInt(rawValue, fullKey);

            if (section == "Run" && key == "nEvents") {
                cfg.run.nEvents = value;
                return;
            }

            if (section == "Pgun" && key == "pdg") {
                cfg.pgun.pdg = value;
                return;
            }

            break;
        }

        case ValueType::Double: {
            const double value = parseDouble(rawValue, fullKey);

            if (section == "Pgun" && key == "energy") {
                cfg.pgun.energy = value;
                return;
            }

            if (section == "Pgun" && key == "mass") {
                cfg.pgun.mass = value;
                return;
            }

            if (section == "Geant4.AngularFilter" && key == "targetX") {
                cfg.geant4.angularFilter.targetX = value;
                return;
            }

            if (section == "Geant4.AngularFilter" && key == "targetY") {
                cfg.geant4.angularFilter.targetY = value;
                return;
            }

            if (section == "Geant4.AngularFilter" && key == "targetZ") {
                cfg.geant4.angularFilter.targetZ = value;
                return;
            }

            if (section == "Geant4.AngularFilter" && key == "theta") {
                cfg.geant4.angularFilter.theta = value;
                return;
            }

            if (section == "Geant4.Geometry.RealisticMap" && key == "concreteHalfX") {
                cfg.geant4.realisticMap.concreteHalfX = value;
                return;
            }

            if (section == "Geant4.Geometry.RealisticMap" && key == "frontPbHalfZ") {
                cfg.geant4.realisticMap.frontPbHalfZ = value;
                return;
            }

            break;
        }

        case ValueType::Bool: {
            const bool value = parseBool(rawValue, fullKey);

            if (section == "Geant4" && key == "ui") {
                cfg.geant4.ui = value;
                return;
            }

            if (section == "Geant4.AngularFilter" && key == "enabled") {
                cfg.geant4.angularFilter.enabled = value;
                return;
            }
            
            break;
        }
    }

    throw std::runtime_error(
        "Internal parser error: no assignment rule for key " + fullKey
    );
}

} // anonymous namespace

SimulationConfig loadConfig(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open config file: " + filename);
    }

    SimulationConfig cfg;

    std::unordered_set<std::string> seenKeys;

    std::string currentSection;
    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        lineNumber++;

        line = removeInlineComment(line);
        line = trim(line);

        if (line.empty()) {
            continue;
        }

        if (line.front() == '[' && line.back() == ']') {
            currentSection = trim(line.substr(1, line.size() - 2));

            if (currentSection.empty()) {
                throw std::runtime_error(
                    "Empty section name in " + filename +
                    " at line " + std::to_string(lineNumber)
                );
            }

            continue;
        }

        const std::size_t equalPos = line.find('=');

        if (equalPos == std::string::npos) {
            throw std::runtime_error(
                "Expected key = value in " + filename +
                " at line " + std::to_string(lineNumber)
            );
        }

        if (currentSection.empty()) {
            throw std::runtime_error(
                "Key found outside any section in " + filename +
                " at line " + std::to_string(lineNumber)
            );
        }

        const std::string key = trim(line.substr(0, equalPos));
        const std::string rawValue = trim(line.substr(equalPos + 1));

        if (key.empty()) {
            throw std::runtime_error(
                "Empty key in " + filename +
                " at line " + std::to_string(lineNumber)
            );
        }

        if (rawValue.empty()) {
            throw std::runtime_error(
                "Empty value for key '" + key + "' in " + filename +
                " at line " + std::to_string(lineNumber)
            );
        }

        const ConfigKey* allowed = findAllowedKey(currentSection, key);

        if (!allowed) {
            throw std::runtime_error(
                "Unknown config key '" +
                makeKey(currentSection, key) +
                "' in " + filename +
                " at line " + std::to_string(lineNumber)
            );
        }

        const std::string fullKey = makeKey(currentSection, key);

        if (seenKeys.find(fullKey) != seenKeys.end()) {
            throw std::runtime_error(
                "Duplicated config key '" + fullKey +
                "' in " + filename +
                " at line " + std::to_string(lineNumber)
            );
        }

        seenKeys.insert(fullKey);

        assignValue(
            cfg,
            currentSection,
            key,
            rawValue,
            allowed->type
        );
    }

    return cfg;
}

} // namespace config