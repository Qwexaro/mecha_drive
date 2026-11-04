#pragma once

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>

#include <memory>
#include <string>
#include <functional>
#include <iostream>
#include <unordered_map>

class Maru {

    std::unique_ptr<td::ClientManager> _client_manager;

    int32_t _client_id = 0;

    int _need_restart = 0;

    void sendQuery(td::td_api::object_ptr<td::td_api::Function> f, std::function<void(td::td_api::object_ptr<td::td_api::Object>)> handler = nullptr);

    void processResponse(td::ClientManager::Response response);

    void onAuthorizationStateUpdate(td::td_api::object_ptr<td::td_api::AuthorizationState> state);

    void onNewMessage(td::td_api::object_ptr<td::td_api::message> message);

    void registerCommand(const std::string& name, CommandHandler handler);

    void setupCommands();

    void editMessageText(int64_t chat_id, int64_t message_id, const std::string& new_text);

    using commandHandler = std::function<void(const td::td_api::message&, const std::string&)>;

    std::unordered_map<std::string, CommandHandler> _commands;

public:

    Maru();

    void run();

};
