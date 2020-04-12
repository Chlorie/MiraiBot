#include <iostream>
#include <mirai/mirai.h>

int main()
{
    const char* auth_key = "somerandomauthkeyformybot";
    const int64_t qq = 3378448768;
    const int64_t dev = 1357008522;
    const int64_t dnd = 820594943;

    try
    {
        mirai::Session sess(auth_key, qq);
        sess.config(mirai::SessionConfig{ 4096, true });
        sess.start_websocket_client();
        std::vector<int32_t> v;
        auto& connection = sess.subscribe_all_events([&](const mirai::Event& e)
        {
            const mirai::GroupRecallEvent* eptr = e.get_if<mirai::GroupRecallEvent>();
            if (!eptr) return;
            if (const auto iter = std::find(v.begin(), v.end(), eptr->message_id);
                iter != v.end())
                v.erase(iter);
            else
            {
                const auto& msg = sess.message_from_id(eptr->message_id)
                    .get<mirai::EventType::group_message>()
                    .message;
                std::string str;
                for (auto& chain : msg)
                {
                    const auto ptr = chain.get_if<mirai::MessageChainNodeType::plain>();
                    if (ptr) str += ptr->text;
                }
                sess.send_group_message(eptr->group.id, str, eptr->message_id);
                v.push_back(eptr->message_id);
            }
        }, mirai::error_logger);
        std::string line;
        while (true)
        {
            std::getline(std::cin, line);
            if (line == "stop") break;
        }
        if (connection.ended())
            throw mirai::RuntimeError(connection.error_reason());
    }
    catch (const mirai::RuntimeError& e)
    {
        std::cerr << mirai::utils::utf8_to_local(e.what()) << '\n';
    }

    return 0;
}
