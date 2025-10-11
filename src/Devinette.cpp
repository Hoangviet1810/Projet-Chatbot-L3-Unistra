#include "Devinette.hpp"
#include <iostream>
#include <cpr/cpr.h>
#include <cpr/util.h>
#include <nlohmann/json.hpp>
#include <random>
#include <set>


using json = nlohmann::json;

Devinette::Devinette() : bot("http://llmcode.math.unistra.fr:8090/v1/chat/completions") {} // on peut changer le link de d'autre chatbot IA (chatgpt, minsitral, deepseek, etc ) pour continuer.

std::string Devinette::choisirMotAleatoire() {
   std::string url = "https://random-words-api.vercel.app/word/french";  // une site pour obtenir un mot aléatoire mais avec de sens
   cpr::Response r = cpr::Get(cpr::Url{url});
                               
    if (r.status_code != 200) {
        std::cerr << "Erreur de connexion à l'API" << std::endl; 
        return "";
    }

    try {
        auto json_response = nlohmann::json::parse(r.text);
        if (!json_response.empty()) {
            if (json_response.contains("word")) {
                std::string mot = json_response["word"];
                // Si mot vide ou suspect
                if (mot.empty() || mot == "null") {
                    std::cerr << "⚠️ Mot vide reçu de l'API." << std::endl;
                    return "";
                }
                return mot;
            } else if (json_response[0].contains("word")) {
                    return json_response[0]["word"];
                }
            } 
            
    } catch (const std::exception& e) {
        std::cerr << "❌ Erreur lors du traitement JSON : " << e.what() << std::endl;
        std::cerr << "Réponse brute : " << r.text << std::endl;
    }

    return "";
}

std::string Devinette::obtenirIndice(const std::string& mot_secret, int index) {
    std::string base_url = "https://api.arasaac.org/api/pictograms/fr/bestsearch/"; 
    std::string url = base_url + cpr::util::urlEncode(mot_secret);
    cpr::Response r = cpr::Get(cpr::Url{url});

    if (r.status_code != 200) {
        std::cerr << "Erreur de connexion" << std::endl;
        return "❌ Aucun indice disponible.";
    }
    
    try {
        auto json_data = json::parse(r.text);
        if (json_data.empty()) {
            return "❌ Không tìm thấy kết quả cho từ khóa.";
        }
        std::set<std::string> all_tags;
        std::set<std::string> all_categories;
        
        for (const auto& item : json_data) {
            if (item.contains("tags") && item["tags"].is_array()) {
                for (const auto& tag : item["tags"]) {
                    all_tags.insert(tag.get<std::string>());
                }
            }

            if (item.contains("categories") && item["categories"].is_array()) {
                for (const auto& cat : item["categories"]) {
                    all_categories.insert(cat.get<std::string>());
                }
            }
        }
        std::string resultat;
        if (!all_tags.empty()) {
            resultat += "Tags: ";
            for (const auto& tag : all_tags) {
                resultat += tag + ", ";
            }
            resultat += "\n";
        }
        if (!all_categories.empty()) {
            resultat += "Categories: ";
            for (const auto& cat : all_categories) {
                resultat += cat + ", ";
            }
            resultat += "\n";
        }
        if (resultat.empty()) {
            return "❌ Aucun indice disponible pour ce mot.";
        }
        std::cout << "-----------------------\n";
        return resultat;
    } catch (const std::exception& e) {
        std::cerr << "Erreur JSON : " << e.what() << std::endl;
    }

    return "❌ Aucun indice disponible pour ce mot.";
}

void Devinette::lancer() {
    std::cout << "Bienvenue dans le jeu de devinettes !" << std::endl;
    char choix_mode;

    std::cout << "Choisissez le mode de jeu : " << std::endl;
    std::cout << "1. Le joueur devine le mot du chatbot (Mode 1)" << std::endl;
    std::cout << "2. Le chatbot devine le mot du joueur (Mode 2)" << std::endl;
    std::cout << "Entrez 1 ou 2 : ";
    std::cin >> choix_mode;
    std::cin.ignore();

    if (choix_mode == '1') {
        modeJoueurDevine();
    } else if (choix_mode == '2') {
        modeChatbotDevine();
    } else {
        std::cout << "Choix invalide. Veuillez Veuillez entrer 1 ou 2." << std::endl;
    }

    char choix_rejouer;
    std::cout << "Souhaitez-vous rejouer ? (o/n) ";
    std::cin >> choix_rejouer;
    if (choix_rejouer == 'o' || choix_rejouer == 'O') {
        lancer();
    }
}

// Mode 1 : Le joueur devine
// Le chatbot pense à un mot et le joueur doit poser des questions pour deviner ce mot.
// Le chatbot répond par "oui", "non".
// Le joueur peut taper "gagné" pour arrêter le jeu.

void Devinette::modeJoueurDevine() {
    // Chatbot choisit le mot secret
    std::string mot_secret = choisirMotAleatoire(); 

    if (mot_secret.empty()) {
        std::cerr << "❌ Aucun mot n'a été récupéré. Vérifiez l'API." << std::endl;
        return;
    }

    //std::cout << "[DEBUG] Le mot choisi : " << mot_secret << std::endl;
    std::cout << "Le mot secret a été choisi. Essayez de le deviner !" << std::endl;
    
    std::cout << "--- Mode 1 : Le chatbot pense à un mot ---" << std::endl;
    std::cout << "Posez vos questions (tapez mot que vous avez choisit ou 'echec' pour arrêter, 'indic1' pour obtenir une aide, indic2 pour obtenir une aide en anglais) :" << std::endl;
    std::cout << "(Exemples : Est-ce que c’est un animal ? / Est-ce que c’est quelque chose que je peux manger ?)" << std::endl;

    std::vector<json> historiqueMessages = {
        {
            {"role", "system"},
            {"content", "Tu es un chatbot qui joue à un jeu de devinettes. Le mot secret auquel tu penses est \"" + mot_secret + "\". Analyse toujours le mot." +
            " L'utilisateur va poser des questions pour deviner ce mot en fonction du mot secret. Tu dois toujours répondre par 'oui' ou 'non'." +
            " Tu dois analyser le sens des questions et répondre logiquement, comme un humain qui connaît le mot. Si l'utilisateur tape 'indic1'," +
            "donne un indice utile mais ne révèle pas le mot sans révéler le mot. IMPORTANT: Ne révèle jamais le mot secret. Ne le mentionne pas, même dans un indice."}
        }                                                                                                  
    };
    
    
    int indexIndice = 0;  
    int nb_indice = 1; 
    std::string question;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, question);

        if (question == mot_secret) {
            std::cout << "Bravo ! Vous avez deviné !" << std::endl;
            break;
        } else if (question == "echec"){
            std::cout << "Desole ! à la prochaine fois !" << std::endl;
            std::cout << "le mot secret etait : " << mot_secret << std::endl;
            break;  
            
        } else if (question == "indic1") {
            historiqueMessages.push_back({
                {"role", "user"},
                //{"content", "Donne-moi un indice, une définition simple du mot secret, sans le révéler directement. C'est la " + std::to_string(nb_indice++) + "e fois que je demande. L'indice doit être simple, clair, et donner une idée de la catégorie ou de l'utilisation du mot, sans dire le mot directement."}
                {"content", 
                  "Donne-moi un nouvel indice sur le mot secret. L'indice doit commencer par une phrase comme \"C'est un mot qui désigne...\" ou \"Ce mot est utilisé pour parler de...\". "
                  "Cette fois, l'indice doit parler de sa définition sans le révéler directement, proposer un synonyme possible ou décrire une situation typique où ce mot est utilisé. "
                  "Évite de répéter les anciens indices. C'est la " + std::to_string(nb_indice++) + "e fois que je demande. "
                  "IMPORTANT : Ne révèle jamais le mot secret. Ne le mentionne pas, même partiellement ou dans un exemple. Sois subtil, clair, mais jamais trop explicite."}
            });
        
            json data;
            data["messages"] = historiqueMessages;
            data["temperature"] = 0.7;
            data["max_tokens"] = 150;
            data["model"] = "gpt-3.5-turbo";
        
            std::string reponse_raw = bot.poserQuestion(data);
        
            try {
                json reponse_json = json::parse(reponse_raw);
                std::string indice = reponse_json["choices"][0]["message"]["content"];
                std::cout << "🔍 Indice: " << indice << std::endl;
        
                historiqueMessages.push_back({
                    {"role", "assistant"},
                    {"content", indice}
                });
        
            } catch (const std::exception& e) {
                std::cout << "Erreur lors du traitement JSON pour l'indice : " << e.what() << std::endl;
            }
        
            continue;
        }
         
         else if (question == "indic2") {
            std::string indice = obtenirIndice(mot_secret, indexIndice++);
            std::cout << "🔍 Indice: " << indice << std::endl;
            continue;
        }

        // mettre la question dans l'historique
        historiqueMessages.push_back({
            {"role", "user"},
            {"content", question}
        });

        // Préparer les messages pour le chatbot
        json data;
        data["messages"] = historiqueMessages;
        data["temperature"] = 0.7;
        data["max_tokens"] = 150;
        data["model"] = "gpt-3.5-turbo";

        std::string reponse_raw = bot.poserQuestion(data);

        try {
            json reponse_json = json::parse(reponse_raw);

            if (reponse_json.contains("error")) {
                std::cout << "❌ Erreur: " << reponse_json["error"]["message"] << std::endl;
            } else {
                std::string reponse_chatbot = reponse_json["choices"][0]["message"]["content"];
                std::cout << "Chatbot: " << reponse_chatbot << std::endl;
            
                // Ajouter la réponse du chatbot à l'historique
                historiqueMessages.push_back({
                    {"role", "assistant"},
                    {"content", reponse_chatbot}
                 });
            }

        } catch (const std::exception& e) {
            std::cout << "Erreur lors du traitement de la réponse JSON : " << e.what() << std::endl;
            std::cout << "Réponse brute : " << reponse_raw << std::endl;
        }
    }
}



// Mode 2 : Le chatbot devine
// Le joueur pense à un mot et le chatbot doit poser des questions pour deviner ce mot.
// Le joueur répond par "oui", "non".
// Le joueur peut taper "gagné" pour arrêter le jeu.

void Devinette::modeChatbotDevine() {
    std::cout << "--- Mode 2 : C'est le chatbot qui devine ---" << std::endl;
    std::cout << "Pensez à un mot. Le chatbot va essayer de le deviner." << std::endl;

    std::string mot_secret;
    std::cout << "(Pour tester : entrez votre mot secret ici, ne pas le montrer au chatbot) > ";
    std::cin >> mot_secret;

    std::string reponse;
    std::string questions;

    while (true) {
        // Préparer les messages pour le chatbot
        json data;
        data["messages"] = {
            {
                {"role", "system"},
                {"content",
                 "Tu es un chatbot qui joue à un jeu de devinettes.\n"
                 "Ton objectif est de deviner un mot secret pensé par l'utilisateur, mais tu ne dois jamais proposer une réponse directe (comme 'Est-ce que le mot est un chien ?').\n"
                 "❌ Tu ne dois JAMAIS essayer de deviner ou dire le mot secret.\n"
                 "✅ Tu dois seulement poser des questions fermées de type 'Est-ce que...'.\n"
                 "❌ Tu ne dois jamais poser deux fois la même question ou une variante proche.\n"
                 "✅ Utilise les réponses précédentes pour poser une nouvelle question pertinente.\n"
                 "Continue le jeu en posant une question fermée, sans faire de supposition directe."
                "❌ Tu ne dois jamais poser des questions sur l’origine linguistique, étymologie, ou la langue d’origine du mot.\n"
                "✅ Pose des questions concrètes sur les caractéristiques physiques, la catégorie (animal, objet, vivant...), l’usage, la taille, la forme, etc.\n"
                "❌ Ne pose pas des variantes de la même question (ex: origine latine, romaine, italienne...)\n"
                "✅ Pose une seule question fermée claire et logique à chaque tour, en te basant sur les réponses précédentes.\n"
                "❌ Ne devine jamais directement le mot.\n"
                "Continue le jeu en posant une nouvelle question pertinente au format 'Est-ce que...'"}
            
            },
            {
                {"role", "user"},
                {"content", questions.empty() ? "Devine le mot." : questions}
            }
        };

        data["temperature"] = 0.7;
        data["max_tokens"] = 100;
        data["model"] = "gpt-3.5-turbo";

        // Appeler l'API
        std::string reponse_raw = bot.poserQuestion(data);

        try {
            json reponse_json = json::parse(reponse_raw);

            if (reponse_json.contains("error")) {
                std::cout << "❌ Erreur: " << reponse_json["error"]["message"] << std::endl;
                break;
            }

            std::string proposition = reponse_json["choices"][0]["message"]["content"];
            std::cout << "Chatbot : " << proposition << std::endl;

            // Si le chatbot répète une question, on arrête
            if (questions.find(proposition) != std::string::npos) {
                std::cout << "❌ Le chatbot a répété une question déjà posée. Fin du jeu." << std::endl;
                break;
            }

            std::cout << "Répondez (oui/non) : ";
            std::cin >> reponse;

            // Réponse valide ?
            if (reponse == "oui" || reponse == "non") {
                questions += proposition + ": " + reponse + "\n";

                // Le mot secret est détecté dans la question ET la réponse est "oui"
                if (proposition.find(mot_secret) != std::string::npos && reponse == "oui") {
                    std::cout << "🎉 Le chatbot a trouvé ! C'était '" << mot_secret << "' !" << std::endl;
                    break;
                }
            } else {
                std::cout << "❌ Réponse invalide. Essayez 'oui' ou 'non'." << std::endl;
            }

        } catch (const std::exception& e) {
            std::cout << "❌ Erreur lors du traitement de la réponse JSON : " << e.what() << std::endl;
            std::cout << "ℹ️ Réponse brute : " << reponse_raw << std::endl;
            break;
        }
    }
}
