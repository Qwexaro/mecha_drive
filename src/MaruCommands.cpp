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

static std::string toLowerUtf8(const std::string& str) {

    std::string result = "";

    result.reserve(str.size());

    for (size_t i = 0; i < str.size(); ++i) {

        unsigned char c1 = static_cast<unsigned char>(str.at(i));

        if (c1 < 128) {

            result += static_cast<char>(std::tolower(c1));

        } else if (c1 == 0xD0 && i + 1 < str.size()) {

            unsigned char c2 = static_cast<unsigned char>(str.at(i + 1));

            if (c2 >= 0x90 && c2 <= 0xAF) {

                result += static_cast<char>(0xD0);

                result += static_cast<char>(c2 + 0x20);

            } else if (c2 >= 0xB0 && c2 <= 0xBF) {

                result += static_cast<char>(0xD0);

                result += static_cast<char>(c2);

            } else if (c2 == 0x81) {

                result += static_cast<char>(0xD1);

                result += static_cast<char>(0x91);

            } else {

                result += static_cast<char>(c1);

                result += static_cast<char>(c2);

            }

            ++i;

        } else if (c1 == 0xD1 && i + 1 < str.size()) {

            unsigned char c2 = static_cast<unsigned char>(str.at(i + 1));

            if (c2 >= 0x80 && c2 <= 0x8F) {

                result += static_cast<char>(0xD1);

                result += static_cast<char>(c2);

            } else {

                result += static_cast<char>(c1);

                result += static_cast<char>(c2);

            }

            ++i;

        } else {

            result += static_cast<char>(c1);

        }

    }

    return result;

}

void Maru::setupCommands() {

    std::function<void(const td::td_api::message&, const std::string&)> ping_logic = [this](const td::td_api::message& msg, const std::string& args) {

        sendTextMessage(msg.chat_id_, "ПОНГ \U0001F3D3");

    };

    registerCommand("ping", ping_logic);

    registerCommand("пинг", ping_logic);

    std::function<void(const td::td_api::message&, const std::string&)> inf_logic = [this](const td::td_api::message& msg, const std::string& args) {

        int64_t chat_id = msg.chat_id_;

        if (msg.reply_to_ && msg.reply_to_->get_id() == td::td_api::messageReplyToMessage::ID) {

            const td::td_api::messageReplyToMessage& reply_info = static_cast<const td::td_api::messageReplyToMessage&>(*msg.reply_to_);

            int64_t reply_message_id = reply_info.message_id_;

            sendQuery(td::td_api::make_object<td::td_api::getMessage>(chat_id, reply_message_id), [this, chat_id](td::td_api::object_ptr<td::td_api::Object> response) {

                if (response->get_id() != td::td_api::message::ID) {

                    sendTextMessage(chat_id, "\u274C Ошибка получения оригинального сообщения.");

                    return;

                }

                td::td_api::object_ptr<td::td_api::message> replied_msg = td::move_tl_object_as<td::td_api::message>(response);

                if (replied_msg->sender_id_ && replied_msg->sender_id_->get_id() == td::td_api::messageSenderUser::ID) {

                    const td::td_api::messageSenderUser& sender_user = static_cast<const td::td_api::messageSenderUser&>(*replied_msg->sender_id_);

                    int64_t target_user_id = sender_user.user_id_;

                    sendQuery(td::td_api::make_object<td::td_api::getUser>(target_user_id), [this, chat_id](td::td_api::object_ptr<td::td_api::Object> user_response) {

                        if (user_response->get_id() == td::td_api::user::ID) {

                            td::td_api::object_ptr<td::td_api::user> user_info = td::move_tl_object_as<td::td_api::user>(user_response);

                            std::string result = "\u2139 Информация о пользователе:\nИмя: " + user_info->first_name_ + "\nID: " + std::to_string(user_info->id_);

                            sendTextMessage(chat_id, result);

                        } else {

                            sendTextMessage(chat_id, "\u274C Не удалось загрузить данные пользователя.");

                        }

                    });

                } else {

                    sendTextMessage(chat_id, "\u274C Сообщение отправлено не пользователем (возможно, это канал или группа).");

                }

            });

            return;

        }

        sendTextMessage(chat_id, "\u274C Вызовите команду в ответ на сообщение пользователя.");

    };

    registerCommand("inf", inf_logic);

    registerCommand("info", inf_logic);

    registerCommand("инф", inf_logic);

    registerCommand("инфо", inf_logic);

    std::function<void(const td::td_api::message&, const std::string&)> id_logic = [this](const td::td_api::message& msg, const std::string& args) {

        int64_t chat_id = msg.chat_id_;

        if (msg.reply_to_ && msg.reply_to_->get_id() == td::td_api::messageReplyToMessage::ID) {

            const td::td_api::messageReplyToMessage& reply_info = static_cast<const td::td_api::messageReplyToMessage&>(*msg.reply_to_);

            int64_t reply_message_id = reply_info.message_id_;

            sendQuery(td::td_api::make_object<td::td_api::getMessage>(chat_id, reply_message_id), [this, chat_id](td::td_api::object_ptr<td::td_api::Object> response) {

                if (response->get_id() == td::td_api::message::ID) {

                    td::td_api::object_ptr<td::td_api::message> replied_msg = td::move_tl_object_as<td::td_api::message>(response);

                    if (replied_msg->forward_info_ && replied_msg->forward_info_->origin_) {

                        if (replied_msg->forward_info_->origin_->get_id() == td::td_api::messageOriginUser::ID) {

                            const td::td_api::messageOriginUser& origin_user = static_cast<const td::td_api::messageOriginUser&>(*replied_msg->forward_info_->origin_);

                            int64_t replied_to_id = origin_user.sender_user_id_;

                            std::string result = "\U0001F194 Чат: " + std::to_string(chat_id) + "\n\U0001F464 Отправитель пересланного: " + std::to_string(replied_to_id);

                            sendTextMessage(chat_id, result);

                            return;

                        }

                    }

                }

                sendTextMessage(chat_id, "\U0001F194 ID Чата: " + std::to_string(chat_id));

            });

            return;

        }

        sendTextMessage(chat_id, "\U0001F194 ID Чата: " + std::to_string(chat_id));

    };

    registerCommand("id", id_logic);

    registerCommand("ид", id_logic);

}


void Maru::registerCommand(std::string name, std::function<void(const td::td_api::message&, const std::string&)> handler) {

    _commands[std::move(name)] = std::move(handler);

}

void Maru::onNewMessage(td::td_api::object_ptr<td::td_api::message> message) {

    if (message->content_->get_id() == td::td_api::messageText::ID) {

        const td::td_api::messageText& text_content = static_cast<const td::td_api::messageText&>(*message->content_);

        std::string text = text_content.text_->text_;

        if (text.empty()) return;

        size_t space_pos = text.find(' ');

        std::string raw_cmd = (space_pos == std::string::npos) ? text : text.substr(0, space_pos);

        std::string args = (space_pos == std::string::npos) ? "" : text.substr(space_pos + 1);

        if (raw_cmd.starts_with('.') || raw_cmd.starts_with('/')) raw_cmd = raw_cmd.substr(1);

        std::string clean_cmd = toLowerUtf8(raw_cmd);

        std::unordered_map<std::string, std::function<void(const td::td_api::message&, const std::string&)>>::const_iterator it = _commands.find(clean_cmd);

        if (it != _commands.end()) it->second(*message, args);

    }

}
