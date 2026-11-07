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

void Maru::processResponse(td::ClientManager::Response response) {

    if (response.object->get_id() == td::td_api::error::ID) {

        td::td_api::object_ptr<td::td_api::error> error = td::move_tl_object_as<td::td_api::error>(response.object);

        std::cerr << "[Telegram API Error] Код: " << error->code_  << " | Описание: " << error->message_ << std::endl;

        return;

    }

    if (response.request_id == 0) {

        if (response.object->get_id() == td::td_api::updateAuthorizationState::ID) {

            td::td_api::object_ptr<td::td_api::updateAuthorizationState> update = td::move_tl_object_as<td::td_api::updateAuthorizationState>(response.object);

            onAuthorizationStateUpdate(std::move(update->authorization_state_));

        } else if (response.object->get_id() == td::td_api::updateNewMessage::ID) {

            td::td_api::object_ptr<td::td_api::updateNewMessage> update = td::move_tl_object_as<td::td_api::updateNewMessage>(response.object);

            onNewMessage(std::move(update->message_));

        }
    } else {

        std::unordered_map<uint64_t, std::function<void(td::td_api::object_ptr<td::td_api::Object>)>>::const_iterator it = _handlers.find(response.request_id);

        if (it != _handlers.end()) {

            it->second(std::move(response.object));

            _handlers.erase(it);

        }

    }

}

void Maru::onAuthorizationStateUpdate(td::td_api::object_ptr<td::td_api::AuthorizationState> state) {

    switch (state->get_id()) {

    case td::td_api::authorizationStateWaitTdlibParameters::ID: {

        td::td_api::object_ptr<td::td_api::setTdlibParameters> params = td::td_api::make_object<td::td_api::setTdlibParameters>();

        params->use_test_dc_ = false;

        params->database_directory_ = "tdlib_session";

        params->files_directory_ = "tdlib_files";

        params->use_file_database_ = true;

        params->use_chat_info_database_ = true;

        params->use_message_database_ = true;

        params->use_secret_chats_ = false;

        params->api_id_ = API_ID;

        params->api_hash_ = API_HASH;

        params->system_language_code_ = "en";

        params->device_model_ = "C++ UserBot (Maru)";

        params->application_version_ = "1.0";

        sendQuery(std::move(params));

        break;

    }

    case td::td_api::authorizationStateWaitPhoneNumber::ID: {

        std::string phone_number;

        std::cout << "Введите номер телефона (например, +79991112233): ";

        std::cin >> phone_number;

        sendQuery(td::td_api::make_object<td::td_api::setAuthenticationPhoneNumber>(phone_number, nullptr));

        break;

    }

    case td::td_api::authorizationStateWaitPassword::ID: {

        std::string password;

        std::cout << "На аккаунте включен облачный пароль.\nВведите ваш 2FA пароль: ";

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::getline(std::cin, password);

        sendQuery(td::td_api::make_object<td::td_api::checkAuthenticationPassword>(password));

        break;

    }


    case td::td_api::authorizationStateWaitCode::ID: {

        std::string code;

        std::cout << "Введите код подтверждения из Telegram: ";

        std::cin >> code;

        sendQuery(td::td_api::make_object<td::td_api::checkAuthenticationCode>(code));

        break;

    }

    case td::td_api::authorizationStateReady::ID:

        std::cout << "Авторизация успешна! Юзербот готов к работе." << std::endl;

        break;

    case td::td_api::authorizationStateClosing::ID:

        std::cout << "Закрытие сессии..." << std::endl;

        break;

    case td::td_api::authorizationStateClosed::ID:

        std::cout << "Сессия закрыта." << std::endl;

        _need_restart = 1;

        break;

    default:

        std::cout << "Неизвестная операция" << std::endl;

        break;

    }

}
