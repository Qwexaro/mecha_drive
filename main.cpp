/*
 * Copyright (c) 2026 qwexaro (kveks). All rights reserved.
 *
 * Proprietary and Confidential.
 *
 * This source code is the sole intellectual property of the author (qwexaro).
 * It is made publicly visible for educational and auditing purposes only.
 *
 * Unauthorized copying, modification, distribution, or deployment of this file,
 * or any portion of its contents, via any medium, is strictly prohibited.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
 */

#include "Maru.h"

int main() {

    td::ClientManager::execute(td::td_api::make_object<td::td_api::setLogVerbosityLevel>(0));

    Maru maru;

    maru.run();

    return 0;

}
