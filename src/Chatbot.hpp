#ifndef CHATBOT_HPP
#define CHATBOT_HPP

#include <string>
#include <nlohmann/json.hpp>

class Chatbot {
public:
    Chatbot(const std::string& url);
    std::string poserQuestion(const nlohmann::json& requete);

private:
    std::string serveur_url;
};

#endif // CHATBOT_HPP

