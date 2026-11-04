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

#pragma once

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>

#include <memory>
#include <string>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <limits>

class Maru {

    int32_t API_ID;

    std::string API_HASH;

    std::unique_ptr<td::ClientManager> _client_manager;

    int32_t _client_id;

    int _need_restart;

    void sendQuery(td::td_api::object_ptr<td::td_api::Function> f, std::function<void(td::td_api::object_ptr<td::td_api::Object>)> handler = nullptr);

    void processResponse(td::ClientManager::Response response);

    void onAuthorizationStateUpdate(td::td_api::object_ptr<td::td_api::AuthorizationState> state);

    void onNewMessage(td::td_api::object_ptr<td::td_api::message> message);

    using commandHandler = std::function<void(const td::td_api::message&, const std::string&)>;

    void registerCommand(const std::string& name, commandHandler handler);

    void setupCommands();

    void sendTextMessage(int64_t chat_id, const std::string& text);

    void editMessageText(int64_t chat_id, int64_t message_id, const std::string& new_text);

    std::unordered_map<std::string, commandHandler> _commands;

    std::unordered_map<uint64_t, std::function<void(td::td_api::object_ptr<td::td_api::Object>)>> _handlers;

    bool loadConfig(const std::string& filename);

public:

    Maru();

    void run();

};
