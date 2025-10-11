#include "Chatbot.hpp"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Chatbot::Chatbot(const std::string& url) : serveur_url(url) {}

std::string Chatbot::poserQuestion(const json& requete) {
    
    cpr::Response r = cpr::Post(
        cpr::Url{serveur_url},
        cpr::Body{requete.dump()},
        cpr::Header{{"Content-Type", "application/json"}}
    );

    if (r.status_code != 200) {
        json err_json;
        err_json["erreur"] = "Erreur HTTP : " + std::to_string(r.status_code);
        err_json["message"] = r.text;
        return err_json.dump();
    }

    return r.text;
}


