#include "Maru.h"

int main() {

    td::ClientManager::execute(td::td_api::make_object<td::td_api::setLogVerbosityLevel>(0));

    Maru maru;

    maru.run();

    return 0;

}
