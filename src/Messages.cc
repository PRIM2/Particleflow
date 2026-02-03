#include "Messages.hh"
#include <chrono>
#include <ctime>

namespace msg {
    const G4String ERR_INI = "\n======= >> [ERROR] << =======\n";
    const G4String ERR_END = "\n=============================\n";
    const G4String OUT     = "[out] ";

    G4String ExecutionTime() {
        const auto now = std::chrono::system_clock::now();
        const std::time_t t = std::chrono::system_clock::to_time_t(now);

        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
        return G4String(buf);
    }

}
