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

Maru::Maru(): _client_manager{std::make_unique<td::ClientManager>()}, _client_id{_client_manager->create_client_id()}, _need_restart{0}, API_HASH{""}, API_ID{0} {
    setupCommands();
}

bool Maru::loadConfig(const std::string& filename) {

    std::ifstream infile(filename);

    if (!infile.is_open()) {

        std::cerr << "[Ошибка]: Не удалось открыть файл конфигурации " << filename << std::endl << "Убедитесь, что файл создан и лежит в рабочей директории бота." << std::endl;

        return false;

    }

    std::string line;

    while (std::getline(infile, line)) {

        if (!line.empty() && line.back() == '\r') line.pop_back();

        if (line.empty() || line[0] == '#') continue;

        std::size_t delimiter_pos = line.find('=');

        if (delimiter_pos == std::string::npos) continue;

        std::string key = line.substr(0, delimiter_pos);

        std::string value = line.substr(delimiter_pos + 1);

        std::size_t key_first = key.find_first_not_of(" \t");

        if (key_first != std::string::npos) {

            std::size_t key_last = key.find_last_not_of(" \t");

            key = key.substr(key_first, (key_last - key_first + 1));

        }

        std::size_t val_first = value.find_first_not_of(" \t");

        if (val_first != std::string::npos) {

            std::size_t val_last = value.find_last_not_of(" \t");

            value = value.substr(val_first, (val_last - val_first + 1));

        }

        if (key == "API_ID") {

            try {

                API_ID = std::stoi(value);

            } catch (...) {

                std::cerr << "[Ошибка]: Не удалось распарсить API_ID как число!" << std::endl;

            }

        }

        else if (key == "API_HASH") API_HASH = value;

    }

    if (API_ID == 0 || API_HASH.empty()) {

        std::cerr << "[Ошибка]: Файл конфигурации заполнен некорректно!" << std::endl;

        return false;

    }

    return true;

}

void Maru::run() {

    if (!loadConfig("config.txt")) {

        std::cerr << "Критическая ошибка. Завершение работы юзербота." << std::endl;

        return;

    }

    std::cout << "Конфигурация успешно загружена." << std::endl << "Юзербот запущен. Инициализация сессии..." << std::endl;

    td::td_api::object_ptr<td::td_api::getOption> get_version_query = td::td_api::make_object<td::td_api::getOption>("version");

    sendQuery(std::move(get_version_query));

    std::cout << "Ожидание событий..." << std::endl;

    while (_need_restart == 0) {

        td::ClientManager::Response response = _client_manager->receive(10.0);

        if (response.object) processResponse(std::move(response));

    }

}

void Maru::sendQuery(td::td_api::object_ptr<td::td_api::Function> f, std::function<void(td::td_api::object_ptr<td::td_api::Object>)> handler) {

    static std::uint64_t current_query_id = 1;

    std::uint64_t query_id = current_query_id++;

    if (handler) _handlers[query_id] = std::move(handler);

    _client_manager->send(_client_id, query_id, std::move(f));

}

void Maru::editMessageText(int64_t chat_id, int64_t message_id, std::string new_text) {

    td::td_api::object_ptr<td::td_api::formattedText> formatted_text = td::td_api::make_object<td::td_api::formattedText>();

    formatted_text->text_ = std::move(new_text);

    td::td_api::object_ptr<td::td_api::inputMessageText> input_content = td::td_api::make_object<td::td_api::inputMessageText>(std::move(formatted_text), nullptr, true);

    sendQuery(td::td_api::make_object<td::td_api::editMessageText>(

                  chat_id,

                  message_id,

                  td::td_api::object_ptr<td::td_api::ReplyMarkup>(),

                  std::move(input_content)

              ));

}

void Maru::sendTextMessage(int64_t chat_id, std::string text) {

    td::td_api::object_ptr<td::td_api::formattedText> formatted_text = td::td_api::make_object<td::td_api::formattedText>();

    formatted_text->text_ = std::move(text);

    td::td_api::object_ptr<td::td_api::inputMessageText> input_content = td::td_api::make_object<td::td_api::inputMessageText>(std::move(formatted_text), nullptr, false);

    sendQuery(td::td_api::make_object<td::td_api::sendMessage>(

                  chat_id,

                  td::td_api::object_ptr<td::td_api::MessageTopic>(),

                  td::td_api::object_ptr<td::td_api::InputMessageReplyTo>(),

                  td::td_api::object_ptr<td::td_api::messageSendOptions>(),

                  td::td_api::object_ptr<td::td_api::ReplyMarkup>(),

                  std::move(input_content)

              ));

}
