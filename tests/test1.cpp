// Test unitaire pour le Chatbot
// Ce test vérifie le bon fonctionnement de la classe Chatbot
#include "../src/Chatbot.hpp"
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Fonction pour afficher le résultat d'un test
void afficherResultat(const std::string& nomTest, bool succes) {
    std::cout << (succes ? "✅ " : "❌ ") << nomTest << std::endl;
}

// Fonction qui vérifie si la réponse JSON est valide (contient "choices")
bool verifierReponseValide(const std::string& reponseBrute) {
    try {
        json parsed = json::parse(reponseBrute);
        if (parsed.contains("choices") && !parsed["choices"].empty()) {
            std::string contenu = parsed["choices"][0]["message"]["content"];
            std::cout << "   Contenu extrait : " << contenu << std::endl;
            return true;
        }
    } catch (...) {
        return false;
    }
    return false;
}

int main() {
    // URL de l'API LLM (changer si besoin)
    std::string api_url = "http://llmcode.math.unistra.fr:8090/v1/chat/completions";
    Chatbot bot(api_url);

    // === Test 1 : Question simple basique ===
    {
        json data;
        data["model"] = "gpt-3.5-turbo";
        data["temperature"] = 0.7;
        data["max_tokens"] = 50;
        data["messages"] = {
            {
                {"role", "system"},
                {"content", "Tu es un assistant utile qui répond de façon concise."}
            },
            {
                {"role", "user"},
                {"content", "Bonjour, comment vas-tu ?"}
            }
        };

        std::cout << "=== Test 1 : Question simple ===\n";
        std::string response = bot.poserQuestion(data);
        bool ok = verifierReponseValide(response);
        afficherResultat("Test 1", ok);
        std::cout << std::endl;
    }

    // === Test 2 : Question demandant une définition ===
    {
        json data;
        data["model"] = "gpt-3.5-turbo";
        data["temperature"] = 0.5;
        data["max_tokens"] = 60;
        data["messages"] = {
            {
                {"role", "system"},
                {"content", "Tu es un assistant pédagogique."}
            },
            {
                {"role", "user"},
                {"content", "Définis la notion d'algorithme."}
            }
        };

        std::cout << "=== Test 2 : Définition d'un concept ===\n";
        std::string response = bot.poserQuestion(data);
        bool ok = verifierReponseValide(response);
        afficherResultat("Test 2", ok);
        std::cout << std::endl;
    }

    // === Test 3 : Question avec demande de liste ===
    {
        json data;
        data["model"] = "gpt-3.5-turbo";
        data["temperature"] = 0.7;
        data["max_tokens"] = 80;
        data["messages"] = {
            {
                {"role", "system"},
                {"content", "Tu es un assistant qui répond clairement."}
            },
            {
                {"role", "user"},
                {"content", "Donne-moi une liste de 3 langages de programmation populaires."}
            }
        };

        std::cout << "=== Test 3 : Liste simple ===\n";
        std::string response = bot.poserQuestion(data);
        bool ok = verifierReponseValide(response);
        afficherResultat("Test 3", ok);
        std::cout << std::endl;
    }

    // === Test 4 : Question technique ===
    {
        json data;
        data["model"] = "gpt-3.5-turbo";
        data["temperature"] = 0.6;
        data["max_tokens"] = 100;
        data["messages"] = {
            {
                {"role", "system"},
                {"content", "Tu es un expert en informatique."}
            },
            {
                {"role", "user"},
                {"content", "Explique comment fonctionne la récursivité."}
            }
        };

        std::cout << "=== Test 4 : Question technique ===\n";
        std::string response = bot.poserQuestion(data);
        bool ok = verifierReponseValide(response);
        afficherResultat("Test 4", ok);
        std::cout << std::endl;
    }

    // === Test 5 : Question philosophique ===
    {
        json data;
        data["model"] = "gpt-3.5-turbo";
        data["temperature"] = 0.8;
        data["max_tokens"] = 90;
        data["messages"] = {
            {
                {"role", "system"},
                {"content", "Tu es un assistant qui aime la philosophie."}
            },
            {
                {"role", "user"},
                {"content", "Qu'est-ce que le libre arbitre ?"}
            }
        };

        std::cout << "=== Test 5 : Question philosophique ===\n";
        std::string response = bot.poserQuestion(data);
        bool ok = verifierReponseValide(response);
        afficherResultat("Test 5", ok);
        std::cout << std::endl;
    }

    // === Test 6 : Température à zéro (réponse déterministe) ===
    {
        json data;
        data["model"] = "gpt-3.5-turbo";
        data["temperature"] = 0.0;
        data["max_tokens"] = 50;
        data["messages"] = {
            {
                {"role", "system"},
                {"content", "Tu es un assistant strictement factuel."}
            },
            {
                {"role", "user"},
                {"content", "Donne-moi la formule chimique de l'eau."}
            }
        };

        std::cout << "=== Test 6 : Température à zéro (réponse déterministe) ===\n";
        std::string response = bot.poserQuestion(data);
        bool ok = verifierReponseValide(response);
        afficherResultat("Test 6", ok);
        std::cout << std::endl;
    }

    // === Test 7 : max_tokens très faible (réponse tronquée possible) ===
    {
        json data;
        data["model"] = "gpt-3.5-turbo";
        data["temperature"] = 0.7;
        data["max_tokens"] = 5;
        data["messages"] = {
            {
                {"role", "system"},
                {"content", "Tu es un assistant concis."}
            },
            {
                {"role", "user"},
                {"content", "Explique la gravité."}
            }
        };

        std::cout << "=== Test 7 : max_tokens très faible ===\n";
        std::string response = bot.poserQuestion(data);
        bool ok = verifierReponseValide(response);
        afficherResultat("Test 7", ok);
        std::cout << std::endl;
    }

    // === Test 8 : Message vide envoyé ===
    {
        json data;
        data["model"] = "gpt-3.5-turbo";
        data["messages"] = {
            {
                {"role", "user"},
                {"content", ""}
            }
        };

        std::cout << "=== Test 8 : Message vide envoyé ===\n";
        std::string response = bot.poserQuestion(data);
        bool ok = verifierReponseValide(response);
        afficherResultat("Test 8", ok);
        std::cout << std::endl;
    }

    // === Test 9 : Plusieurs messages dans la conversation ===
    {
        json data;
        data["model"] = "gpt-3.5-turbo";
        data["temperature"] = 0.7;
        data["max_tokens"] = 60;
        data["messages"] = {
            {
                {"role", "system"},
                {"content", "Tu es un assistant patient et pédagogique."}
            },
            {
                {"role", "user"},
                {"content", "Peux-tu m'expliquer ce qu'est un algorithme ?"}
            },
            {
                {"role", "assistant"},
                {"content", "Bien sûr ! Un algorithme est une suite finie d'instructions..."}
            },
            {
                {"role", "user"},
                {"content", "Peux-tu donner un exemple simple ?" }
            }
        };

        std::cout << "=== Test 9 : Plusieurs messages dans la conversation ===\n";
        std::string response = bot.poserQuestion(data);
        bool ok = verifierReponseValide(response);
        afficherResultat("Test 9", ok);
        std::cout << std::endl;
    }

    // === Test 10 : Message avec rôle assistant uniquement ===
    {
        json data;
        data["model"] = "gpt-3.5-turbo";
        data["messages"] = {
            {
                {"role", "assistant"},
                {"content", "Je suis là pour aider."}
            }
        };

        std::cout << "=== Test 10 : Message avec rôle assistant uniquement ===\n";
        std::string response = bot.poserQuestion(data);
        bool ok = verifierReponseValide(response);
        afficherResultat("Test 10", ok);
        std::cout << std::endl;
    }

    return 0;
}