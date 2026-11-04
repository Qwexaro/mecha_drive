#include "Maru.h"

Maru::Maru(): _client_manager{std::make_unique<td::ClientManager>()}, _client_id{_client_manager->create_client_id()}, _need_restart{0}, API_HASH{""}, API_ID{0} {
    setupCommands();
}

bool Maru::loadConfig(const std::string& filename) {

    std::ifstream infile(filename);

    if (!infile.is_open()) {

        std::cerr << "[Ошибка]: Не удалось открыть файл конфигурации " << filename << std::endl;

        std::cerr << "Убедитесь, что файл создан и лежит в рабочей директории бота." << std::endl;

        return false;

    }

    std::string line;

    while (std::getline(infile, line)) {

        if (line.empty() || line[0] == '#') continue;

        std::size_t delimiter_pos = line.find('=');

        if (delimiter_pos == std::string::npos) continue;

        std::string key = line.substr(0, delimiter_pos);

        std::string value = line.substr(delimiter_pos + 1);

        if (key == "API_ID") API_ID = std::stoi(value);

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

    if (handler) _handlers[query_id] = std::move(handler);

    _client_manager->send(_client_id, query_id, std::move(f));

}

void Maru::processResponse(td::ClientManager::Response response) {

    if (response.object->get_id() == td::td_api::error::ID) {

        auto error = td::move_tl_object_as<td::td_api::error>(response.object);

        std::cerr << "[Telegram API Error] Код: " << error->code_  << " | Описание: " << error->message_ << std::endl;

        return;

    }

    if (response.request_id == 0) {

        if (response.object->get_id() == td::td_api::updateAuthorizationState::ID) {

            auto update = td::move_tl_object_as<td::td_api::updateAuthorizationState>(response.object);

            onAuthorizationStateUpdate(std::move(update->authorization_state_));

        } else if (response.object->get_id() == td::td_api::updateNewMessage::ID) {

            auto update = td::move_tl_object_as<td::td_api::updateNewMessage>(response.object);

            onNewMessage(std::move(update->message_));

        }
    } else {

        auto it = _handlers.find(response.request_id);

        if (it != _handlers.end()) {

            it->second(std::move(response.object));

            _handlers.erase(it);

        }

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
        std::cout << "Неизвестная операция";
        break;

    }

}

void Maru::setupCommands() {

    registerCommand(".ping", [this](const td::td_api::message& msg, const std::string& args) {

        if(!msg.is_outgoing_) return;

        auto formatted_text = td::td_api::make_object<td::td_api::formattedText>();

        formatted_text->text_ = "pong";

        auto input_content = td::td_api::make_object<td::td_api::inputMessageText>(std::move(formatted_text), nullptr, false);

        sendQuery(td::td_api::make_object<td::td_api::sendMessage>(

                      msg.chat_id_, td::td_api::object_ptr<td::td_api::MessageTopic>(),

                      td::td_api::object_ptr<td::td_api::InputMessageReplyTo>(), td::td_api::object_ptr<td::td_api::messageSendOptions>(),

                      td::td_api::object_ptr<td::td_api::ReplyMarkup>(), std::move(input_content)

                  ));



    });

    registerCommand(".hi", [this](const td::td_api::message& msg, const std::string& args) {
        if(msg.is_outgoing_) editMessageText(msg.chat_id_, msg.id_, "Hello World!");
    });

    registerCommand(".id", [this](const td::td_api::message& msg, const std::string& args) {

        if (!msg.is_outgoing_) return;

        int64_t chat_id = msg.chat_id_;

        int64_t my_msg_id = msg.id_;

        if (msg.reply_to_ && msg.reply_to_->get_id() == td::td_api::messageReplyToMessage::ID) {

            const auto& reply_info_base = msg.reply_to_;

            auto& reply_info = static_cast<const td::td_api::messageReplyToMessage&>(*reply_info_base);

            int64_t reply_message_id = reply_info.message_id_;

            sendQuery(td::td_api::make_object<td::td_api::getMessage>(chat_id, reply_message_id), [this, chat_id, my_msg_id](td::td_api::object_ptr<td::td_api::Object> response) {

                if (response->get_id() == td::td_api::message::ID) {

                    td::td_api::object_ptr<td::td_api::message> replied_msg = td::move_tl_object_as<td::td_api::message>(response);

                    if (replied_msg->forward_info_) {

                        td::td_api::object_ptr<td::td_api::messageForwardInfo> f_info = std::move(replied_msg->forward_info_);

                        if (f_info->origin_ && f_info->origin_->get_id() == td::td_api::messageOriginUser::ID) {

                            td::td_api::object_ptr<td::td_api::messageOriginUser> origin_user =
                                td::move_tl_object_as<td::td_api::messageOriginUser>(f_info->origin_);

                            int64_t replied_to_id = origin_user->sender_user_id_;

                            std::string result = "Chat ID: " + std::to_string(chat_id) + ", Replied To ID: " + std::to_string(replied_to_id);

                            editMessageText(chat_id, my_msg_id, result);

                            return;

                        }

                    }

                }

                editMessageText(chat_id, my_msg_id, "Chat ID: " + std::to_string(chat_id));

            });

            return;

        }

        editMessageText(chat_id, my_msg_id, "Chat ID: " + std::to_string(chat_id));

    });


    registerCommand(".inf", [this](const td::td_api::message& msg, const std::string& args) {

        if (!msg.is_outgoing_) return;

        int64_t chat_id = msg.chat_id_;

        int64_t my_msg_id = msg.id_;

        if (msg.reply_to_ && msg.reply_to_->get_id() == td::td_api::messageReplyToMessage::ID) {

            const auto& reply_info_base = msg.reply_to_;

            auto& reply_info = static_cast<const td::td_api::messageReplyToMessage&>(*reply_info_base);

            int64_t reply_message_id = reply_info.message_id_;

            sendQuery(td::td_api::make_object<td::td_api::getMessage>(chat_id, reply_message_id), [this, chat_id, my_msg_id](td::td_api::object_ptr<td::td_api::Object> response) {

                if (response->get_id() == td::td_api::message::ID) {

                    td::td_api::object_ptr<td::td_api::message> replied_msg = td::move_tl_object_as<td::td_api::message>(response);

                    if (replied_msg->sender_id_ && replied_msg->sender_id_->get_id() == td::td_api::messageSenderUser::ID) {

                        auto& sender_user = static_cast<const td::td_api::messageSenderUser&>(*replied_msg->sender_id_);

                        int64_t target_user_id = sender_user.user_id_;

                        sendQuery(td::td_api::make_object<td::td_api::getUser>(target_user_id), [this, chat_id, my_msg_id](td::td_api::object_ptr<td::td_api::Object> user_response) {

                            if (user_response->get_id() == td::td_api::user::ID) {

                                td::td_api::object_ptr<td::td_api::user> user_info = td::move_tl_object_as<td::td_api::user>(user_response);

                                std::string first_name = user_info->first_name_;

                                int64_t user_id = user_info->id_;


                                std::string result = "Информация о пользователе:\nИмя: " + first_name + "\nID: " + std::to_string(user_id);

                                editMessageText(chat_id, my_msg_id, result);

                                return;

                            }

                            editMessageText(chat_id, my_msg_id, "[Ошибка]: Не удалось загрузить карточку пользователя.");

                        });

                        return;

                    }

                }

                editMessageText(chat_id, my_msg_id, "[Ошибка]: Не удалось получить исходное сообщение или его автор — не пользователь.");

            });

            return;

        }

        editMessageText(chat_id, my_msg_id, "[Ошибка]: Команду необходимо вызывать в ответ на сообщение пользователя.");

    });


}

void Maru::registerCommand(const std::string& name, commandHandler handler) {
    _commands[name] = handler;
}

void Maru::editMessageText(int64_t chat_id, int64_t message_id, const std::string& new_text) {

    auto formatted_text = td::td_api::make_object<td::td_api::formattedText>();

    formatted_text->text_ = new_text;

    auto input_content = td::td_api::make_object<td::td_api::inputMessageText>(std::move(formatted_text), nullptr, true);

    sendQuery(td::td_api::make_object<td::td_api::editMessageText>(

                  chat_id,

                  message_id,

                  td::td_api::object_ptr<td::td_api::ReplyMarkup>(),

                  std::move(input_content)

              ));

}

void Maru::onNewMessage(td::td_api::object_ptr<td::td_api::message> message) {

    if (message->content_->get_id() == td::td_api::messageText::ID) {

        auto& text_content = static_cast<const td::td_api::messageText&>(*message->content_);

        std::string text = text_content.text_->text_;

        size_t space_pos = text.find(' ');

        std::string cmd = (space_pos == std::string::npos) ? text : text.substr(0, space_pos);

        std::string args = (space_pos == std::string::npos) ? "" : text.substr(space_pos + 1);

        auto it = _commands.find(cmd);

        if (it != _commands.end()) it->second(*message, args);

    }

}
