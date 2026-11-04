#include "Maru.h"

const int32_t API_ID = API_ID;

const std::string API_HASH = API_HASH;

Maru::Maru(): _client_manager{std::make_unique<td::ClientManager>()}, _client_id{_client_manager->create_client_id()} {}

void Maru::run() {

    std::cout << "Юзербот запущен. Инициализация сессии..." << std::endl;

    auto get_version_query = td::td_api::make_object<td::td_api::getOption>("version");

    sendQuery(std::move(get_version_query));

    std::cout << "Ожидание событий..." << std::endl;

    while (_need_restart == 0) {

        auto response = _client_manager->receive(10.0);

        if (response.object) processResponse(std::move(response));

    }

}

void Maru::sendQuery(td::td_api::object_ptr<td::td_api::Function> f, std::function<void(td::td_api::object_ptr<td::td_api::Object>)> handler) {

    static std::uint64_t current_query_id = 1;

    std::uint64_t query_id = current_query_id++;

    _client_manager->send(_client_id, query_id, std::move(f));

}

void Maru::processResponse(td::ClientManager::Response response) {

    if (response.object->get_id() == td::td_api::updateAuthorizationState::ID) {

        auto update = td::move_tl_object_as<td::td_api::updateAuthorizationState>(response.object);

        onAuthorizationStateUpdate(std::move(update->authorization_state_));

    } else if (response.object->get_id() == td::td_api::updateNewMessage::ID) {

        auto update = td::move_tl_object_as<td::td_api::updateNewMessage>(response.object);

        onNewMessage(std::move(update->message_));

    }

}

void Maru::onAuthorizationStateUpdate(td::td_api::object_ptr<td::td_api::AuthorizationState> state) {

    switch (state->get_id()) {

        case td::td_api::authorizationStateWaitTdlibParameters::ID: {

            auto params = td::td_api::make_object<td::td_api::setTdlibParameters>();

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

        case td::td_api::authorizationStateWaitCode::ID: {

            std::string code;

            std::cout << "Введите код подтверждения из Telegram: ";

            std::cin >> code;

            sendQuery(td::td_api::make_object<td::td_api::checkAuthenticationCode>(code));

            break;

        }

        case td::td_api::authorizationStateReady::ID: std::cout << "Авторизация успешна! Юзербот готов к работе." << std::endl; break;

        case td::td_api::authorizationStateClosing::ID: std::cout << "Закрытие сессии..." << std::endl; break;

        case td::td_api::authorizationStateClosed::ID: std::cout << "Сессия закрыта." << std::endl; _need_restart = 1; break;

        default: std::cout << "Неизвестная операция"; break;

    }

}

void Maru::onNewMessage(td::td_api::object_ptr<td::td_api::message> message) {

    if (message->content_->get_id() == td::td_api::messageText::ID) {

        auto& text_content = static_cast<const td::td_api::messageText&>(*message->content_);

        std::string text = text_content.text_->text_;

        if (text == "!ping") {

            std::cout << "Получена команда !ping в чате " << message->chat_id_ << ". Отвечаю..." << std::endl;

            auto formatted_text = td::td_api::make_object<td::td_api::formattedText>();

            formatted_text->text_ = "pong";

            auto input_message_content = td::td_api::make_object<td::td_api::inputMessageText>(std::move(formatted_text), nullptr, false);

            sendQuery(td::td_api::make_object<td::td_api::sendMessage>(

                message->chat_id_,

                td::td_api::object_ptr<td::td_api::MessageTopic>(),

                td::td_api::object_ptr<td::td_api::InputMessageReplyTo>(),

                td::td_api::object_ptr<td::td_api::messageSendOptions>(),

                td::td_api::object_ptr<td::td_api::ReplyMarkup>(),

                std::move(input_message_content)

            ));

        }

    }

}
