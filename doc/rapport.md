# Rapport Technique — Chatbotl3


## 1.  Objectif du projet
Le projet consiste à développer un chatbot capable de jouer aux devinettes.
Le joueur demande au chatbot de penser à un mot, puis il lui pose des
questions auxquelles le chatbot répond par « oui » ou « non ». Quand le
joueur a trouvé, le chatbot doit répondre « gagné » et proposer une nouvelle
partie où cette fois-ci, c’est au chatbot de deviner un mot choisi par le joueur

---

## 2. Structure
```text
chatbotl3/
|-- .gitignore
|-- CMakeLists.txt
|-- doc/
|   └── rapport.md
|-- src/
|   |-- main.cpp
|   |-- Chatbot.cpp
|   |-- Chatbot.hpp
|   |-- Devinette.cpp
|   └── Devinette.hpp
|-- tests/
|   |-- test1.cpp
|   └── test2.cpp
└── README.md
```

---

## 3. CMakeLists 
Pour pouvoir construire le projet avec CMakeLists, j'ai intégré la bibliothèque JSON pour faciliter la communication de données, CPR pour gérer les requêtes HTTP, et Catch2 afin de pouvoir exécuter et vérifier les tests.
Ce projet utilise plusieurs bibliothèques tierces, intégrées via CMake avec la fonctionnalité FetchContent pour une gestion simple et automatique des dépendances :
- JSON for Modern C++ (nlohmann/json) : pour la manipulation facile et efficace des données JSON.
- CPR (C++ Requests) : une bibliothèque HTTP moderne pour envoyer des requêtes réseau (API).
- Catch2 : un framework de test unitaire léger et moderne utilisé pour écrire et exécuter les tests.
```cmake
# JSON
include(FetchContent)
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

# CPR
FetchContent_Declare(cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG dd967cb48ea6bcbad9f1da5ada0db8ac0d532c06)



# Catch 2 pour faire des tests
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.4
)

FetchContent_MakeAvailable(cpr json Catch2)
```
Le fichier CMakeLists.txt sert à créer deux types d’exécutables principaux :

- ./go pour lancer le jeu Le Jeu de Devinette

- ./test1 et ./test2 pour exécuter les tests automatiques

##### Configuration du projet et compilation
```cmake
include_directories(src)
```
On ajoute le dossier src aux chemins d’inclusion pour que les fichiers .hpp soient trouvés facilement.
```cmake
add_executable(go
    src/Chatbot.cpp
    src/main.cpp
    src/Devinette.cpp)
```
Création de l’exécutable principal go à partir des fichiers sources listés.
```cmake
target_link_libraries(go PRIVATE cpr::cpr nlohmann_json::nlohmann_json)
```
On lie l’exécutable aux bibliothèques CPR et JSON.

##### Configuration des tests 
Activation de la prise en charge des tests et inclusion du module Catch2 pour la découverte automatique des tests.
```cmake
add_executable(test1
    tests/test1.cpp
    src/Chatbot.cpp
    src/Devinette.cpp
)
```
Création d’un exécutable test1 pour les tests, compilé à partir des sources de test et des fichiers sources nécessaires.
```cmake
target_include_directories(test1 PRIVATE src)
target_link_libraries(test1
    PRIVATE Catch2::Catch2WithMain
    PRIVATE cpr::cpr nlohmann_json::nlohmann_json
)
```
On configure les chemins d’inclusion et on lie l’exécutable test1 aux bibliothèques nécessaires, dont Catch2 (avec main intégré), CPR et JSON.
```cmake
target_compile_definitions(test1 PRIVATE TEST_MODE)
```
On définit la macro TEST_MODE pour activer certains membres publics spécifiques au test dans le code.
```cmake
catch_discover_tests(test1)
```
Permet à Catch2 de détecter automatiquement les tests dans l’exécutable test1 pour que CTest puisse les lancer.
--

## 4. Chatbot.cpp 
Ce fichier implémente la classe `Chatbot` qui permet la communication avec un serveur via des requêtes HTTP.

- Le constructeur `Chatbot::Chatbot(const std::string& url)` initialise l'objet avec l'URL du serveur.
- La méthode `poserQuestion` envoie une requête JSON au serveur à l'aide de la bibliothèque **cpr**.
- Elle effectue une requête POST avec un corps JSON et un en-tête `Content-Type: application/json`.
- Si la réponse HTTP n'a pas le code 200 (succès), elle renvoie un message d'erreur formaté en JSON, contenant le code d'erreur et le message du serveur.
- Sinon, elle retourne la réponse reçue du serveur sous forme de texte brut.

Cette approche permet d'envoyer facilement des questions ou des données au serveur et de recevoir une réponse, facilitant ainsi l'interaction entre le chatbot et le backend.
```cpp
Chatbot::Chatbot(const std::string& url) : serveur_url(url) {}
```
Explication :
- Il s'agit du constructeur de la classe Chatbot.
- Il prend une chaîne url comme paramètre (l'adresse du serveur).
- serveur_url(url) initialise l'attribut serveur_url avec la valeur reçue.
```cpp
std::string Chatbot::poserQuestion(const json& requete) {
```
Explication :
- C'est une méthode de la classe Chatbot, appelée poserQuestion.
- Elle prend une requête au format JSON (requete) et retourne une chaîne (std::string), qui contiendra la réponse du serveur.

```cpp
    cpr::Response r = cpr::Post(
        cpr::Url{serveur_url},
        cpr::Body{requete.dump()},
        cpr::Header{{"Content-Type", "application/json"}}
    );
```
Explication :
- On envoie une requête HTTP POST à l’URL du serveur serveur_url :
- Le corps de la requête est le JSON converti en texte (requete.dump()).
- L’en-tête indique que le contenu est en format application/json.
- Cela utilise la bibliothèque CPR (C++ Requests)
```cpp
    if (r.status_code != 200) {
```
On vérifie si le code de retour HTTP est différent de 200 (ce qui signifie que la requête n’a pas réussi).
```cpp
          json err_json;
        err_json["erreur"] = "Erreur HTTP : " + std::to_string(r.status_code);
        err_json["message"] = r.text;
        return err_json.dump();
```
Explication :
- Si la requête a échoué, on crée un objet JSON contenant :
   - "erreur" : une chaîne avec le code d’erreur.
   - "message" : le message de réponse (le texte brut de la réponse).
- Ensuite, on retourne ce message d’erreur sous forme de texte JSON.
```cpp
       return r.text;
```
Si la requête a réussi (statut 200), on retourne simplement le texte brut de la réponse.

##### Résumé
La méthode poserQuestion() envoie une requête POST contenant du JSON à un serveur, et retourne la réponse du serveur. En cas d’échec, elle retourne une erreur formatée en JSON.

---
## 5. Devienette.pp
Ce fichier implémente la classe `Devinette` qui gère la logique principale du jeu de devinettes.
Afin d’assurer que le bot choisisse un mot secret valide et ayant un sens, j’ai intégré une API qui fournit aléatoirement un mot existant avec une définition
Tout d'abord, je me connecte à ce site web pour choisir un mot ayant un sens (https://random-words-api.vercel.app/word/french)
La fonction Devinette::choisirMotAleatoire() permet de récupérer un mot français aléatoire ayant un sens, en se connectant à une API publique via HTTP. Elle utilise la bibliothèque cpr pour faire la requête et nlohmann::json pour lire la réponse au format JSON.
##### Détail ligne par ligne
```cpp
      std::string Devinette::choisirMotAleatoire() {
```
- Déclaration d’une méthode (fonction membre) de la classe Devinette.
- Elle retourne une chaîne de caractères (std::string), qui sera le mot choisi

```cpp
       std::string url = "https://random-words-api.vercel.app/word/french";
```
- Définition de l’URL de l’API. Ce site fournit un mot français au hasard avec sa signification.

- Si l’objet JSON n’est pas vide et qu’il contient une clé "word", on extrait le mot.
```cpp
   std::string Devinette::choisirMotAleatoire() {
    std::string url = "https://random-words-api.vercel.app/word/french";  // URL de l'API pour obtenir un mot français aléatoire avec un sens
    cpr::Response r = cpr::Get(cpr::Url{url});  // Envoi d'une requête GET à l'URL

    if (r.status_code != 200) {
        std::cerr << "Erreur de connexion à l'API" << std::endl;  // Affiche une erreur si la réponse HTTP n'est pas OK (200)
        return "";  // Retourne une chaîne vide en cas d'erreur de connexion
    }

    try {
        auto json_response = nlohmann::json::parse(r.text);  // Analyse la réponse JSON reçue
        if (!json_response.empty()) {  // Vérifie que la réponse JSON n'est pas vide
            if (json_response.contains("word")) {  // Si l'objet JSON contient une clé "word"
                std::string mot = json_response["word"];  // Récupère la valeur associée à "word"
                if (mot.empty() || mot == "null") {  // Vérifie si le mot est vide ou égal à la chaîne "null"
                    std::cerr << "⚠️ Mot vide reçu de l'API." << std::endl;  // Avertissement : mot invalide reçu
                    return "";  // Retourne une chaîne vide si le mot n'est pas valide
                }
                return mot;  // Retourne le mot valide
            } else if (json_response[0].contains("word")) {  // Sinon, si la réponse est un tableau JSON et le premier élément contient "word"
                return json_response[0]["word"];  // Retourne le mot contenu dans le premier élément du tableau
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "❌ Erreur lors du traitement JSON : " << e.what() << std::endl;  // Affiche l'exception si le parsing JSON échoue
        std::cerr << "Réponse brute : " << r.text << std::endl;  // Affiche la réponse brute pour aider au débogage
    }

    return "";  // Retourne une chaîne vide si aucun mot valide n'a été obtenu
}
 
```
Après avoir obtenu le mot secret, je me suis connecté à l’API d’un site de dictionnaire français afin de récupérer la définition de ce mot, que j’utiliserai ensuite comme indication pour le joueur. Pour obtenir l'API de ce site (https://dico.apiceras.ch/) on tape F12, tape console et cherche le link(https://api.arasaac.org/api/pictograms/fr/bestsearch/)

```cpp
std::string Devinette::obtenirIndice(const std::string& mot_secret, int index) {
    // Définition de l'URL de base de l'API pour rechercher un mot en français
    std::string base_url = "https://api.arasaac.org/api/pictograms/fr/bestsearch/"; 
    // Construction de l'URL complète en encodant le mot secret
    std::string url = base_url + cpr::util::urlEncode(mot_secret);
    // Envoi de la requête GET à l'URL
    cpr::Response r = cpr::Get(cpr::Url{url});

    // Vérification du code de statut HTTP de la réponse
    if (r.status_code != 200) {
        std::cerr << "Erreur de connexion" << std::endl; // Affiche un message d'erreur si la connexion échoue
        return "❌ Aucun indice disponible.";              // Retourne un message indiquant qu'aucun indice n'est disponible
    }
    
    try {
        // Parsing du texte JSON reçu dans la réponse
        auto json_data = json::parse(r.text);
        // Vérifie si la réponse JSON est vide
        if (json_data.empty()) {
            return "❌ Không tìm thấy kết quả cho từ khóa.";  // Message si aucun résultat n'est trouvé (en vietnamien: "Không tìm thấy kết quả cho từ khóa.")
        }
        // Création de sets pour stocker les tags et catégories sans doublons
        std::set<std::string> all_tags;
        std::set<std::string> all_categories;
        
        // Parcours de chaque élément dans la réponse JSON
        for (const auto& item : json_data) {
            // Si l'élément contient un tableau "tags"
            if (item.contains("tags") && item["tags"].is_array()) {
                // Ajoute chaque tag dans le set all_tags
                for (const auto& tag : item["tags"]) {
                    all_tags.insert(tag.get<std::string>());
                }
            }

            // Si l'élément contient un tableau "categories"
            if (item.contains("categories") && item["categories"].is_array()) {
                // Ajoute chaque catégorie dans le set all_categories
                for (const auto& cat : item["categories"]) {
                    all_categories.insert(cat.get<std::string>());
                }
            }
        }

        // Chaîne de caractères qui va contenir le résultat formaté
        std::string resultat;
        // Si la liste des tags n'est pas vide
        if (!all_tags.empty()) {
            resultat += "Tags: ";
            // Concatène tous les tags dans la chaîne résultat, séparés par des virgules
            for (const auto& tag : all_tags) {
                resultat += tag + ", ";
            }
            resultat += "\n";  // Nouvelle ligne après la liste des tags
        }
        // Si la liste des catégories n'est pas vide
        if (!all_categories.empty()) {
            resultat += "Categories: ";
            // Concatène toutes les catégories dans la chaîne résultat, séparées par des virgules
            for (const auto& cat : all_categories) {
                resultat += cat + ", ";
            }
            resultat += "\n";  // Nouvelle ligne après la liste des catégories
        }

        // Si aucune information n'a été récupérée, renvoie un message d'erreur
        if (resultat.empty()) {
            return "❌ Aucun indice disponible pour ce mot.";
        }

        // Affichage d'une ligne de séparation dans la console (facultatif)
        std::cout << "-----------------------\n";
        // Retourne la chaîne de résultat contenant les tags et catégories
        return resultat;
    } catch (const std::exception& e) {
        // Capture et affiche une erreur si le parsing JSON échoue
        std::cerr << "Erreur JSON : " << e.what() << std::endl;
    }

    // En cas d'erreur ou d'exception, retourne un message d'absence d'indice
    return "❌ Aucun indice disponible pour ce mot.";
}
```
Et après, on lance le jeu 
```cpp
void Devinette::lancer() {
    std::cout << "Bienvenue dans le jeu de devinettes !" << std::endl;  // Affiche un message de bienvenue

    char choix_mode;  // Variable pour stocker le choix du mode de jeu

    std::cout << "Choisissez le mode de jeu : " << std::endl;  // Demande à l'utilisateur de choisir un mode
    std::cout << "1. Le joueur devine le mot du chatbot (Mode 1)" << std::endl;  // Option 1
    std::cout << "2. Le chatbot devine le mot du joueur (Mode 2)" << std::endl;  // Option 2
    std::cout << "Entrez 1 ou 2 : ";  // Invite à entrer 1 ou 2
    std::cin >> choix_mode;  // Lit la saisie utilisateur
    std::cin.ignore();  // Ignore le reste de la ligne (utile pour éviter les problèmes avec getline plus tard)

    // Vérifie la valeur saisie et appelle la fonction correspondante
    if (choix_mode == '1') {
        modeJoueurDevine();  // Lance le mode où le joueur doit deviner le mot choisi par le chatbot
    } else if (choix_mode == '2') {
        modeChatbotDevine();  // Lance le mode où le chatbot doit deviner le mot choisi par le joueur
    } else {
        std::cout << "Choix invalide. Veuillez entrer 1 ou 2." << std::endl;  // Message d'erreur si le choix est incorrect
    }

    char choix_rejouer;  // Variable pour stocker la réponse à la question de rejouer
    std::cout << "Souhaitez-vous rejouer ? (o/n) ";  // Demande à l'utilisateur s'il veut rejouer
    std::cin >> choix_rejouer;  // Lit la réponse
    if (choix_rejouer == 'o' || choix_rejouer == 'O') {
        lancer();  // Relance le jeu si l'utilisateur répond oui
    }
}
```

### Il y 2 mode de joue:
- Mode 1 : Le joueur devine
- Mode 2 : Le chatbot devine

#### Mode 1: Le joueur devine
```text
Bienvenue dans le jeu de devinettes !
Choisissez le mode de jeu : 
1. Le joueur devine le mot du chatbot (Mode 1)
2. Le chatbot devine le mot du joueur (Mode 2)
Entrez 1 ou 2 : 1
Le mot secret a été choisi. Essayez de le deviner !
--- Mode 1 : Le chatbot pense à un mot ---
Posez vos questions (tapez mot que vous avez choisit ou 'echec' pour arrêter, 'indic1' pour obtenir une aide, indic2 pour obtenir une aide en anglais) :
(Exemples : Est-ce que c’est un animal ? / Est-ce que c’est quelque chose que je peux manger ?)
> 
```
---
```cpp
void Devinette::modeJoueurDevine() {
    // Chatbot choisit un mot secret aléatoire
    std::string mot_secret = choisirMotAleatoire(); 

    // Si aucun mot n'a été récupéré, afficher une erreur et quitter la fonction
    if (mot_secret.empty()) {
        std::cerr << "❌ Aucun mot n'a été récupéré. Vérifiez l'API." << std::endl;
        return;
    }

    std::cout << "[DEBUG] Le mot choisi : " << mot_secret << std::endl;
    std::cout << "Le mot secret a été choisi. Essayez de le deviner !" << std::endl;
    
    std::cout << "--- Mode 1 : Le chatbot pense à un mot ---" << std::endl;
    std::cout << "Posez vos questions (tapez mot que vous avez choisi ou 'echec' pour arrêter, 'indic1' pour obtenir une aide, indic2 pour obtenir une aide en anglais) :" << std::endl;
    std::cout << "(Exemples : Est-ce que c’est un animal ? / Est-ce que c’est quelque chose que je peux manger ?)" << std::endl;

    // Historique des messages envoyés au chatbot pour contexte
    std::vector<json> historiqueMessages = {
        {
            {"role", "system"},
            {"content", "Tu es un chatbot qui joue à un jeu de devinettes. Le mot secret auquel tu penses est \"" + mot_secret + "\". Analyse toujours le mot."
             " L'utilisateur va poser des questions pour deviner ce mot en fonction du mot secret. Tu dois toujours répondre par 'oui' ou 'non'."
             " Tu dois analyser le sens des questions et répondre logiquement, comme un humain qui connaît le mot. Si l'utilisateur tape 'indic1',"
             " donne un indice utile mais ne révèle pas le mot sans révéler le mot. IMPORTANT: Ne révèle jamais le mot secret. Ne le mentionne pas, même dans un indice."}
        }                                                                                                  
    };
    
    int indexIndice = 0;   // Compteur pour les indices
    int nb_indice = 1;     // Compteur du nombre de fois que l'utilisateur demande un indice
    std::string question;

    // Boucle principale d'interaction avec le joueur
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, question);

        // Si le joueur devine le mot secret
        if (question == mot_secret) {
            std::cout << "Bravo ! Vous avez deviné !" << std::endl;
            break;
        } 
        // Si le joueur abandonne
        else if (question == "echec"){
            std::cout << "Desole ! à la prochaine fois !" << std::endl;
            std::cout << "le mot secret etait : " << mot_secret << std::endl;
            break;  
        } 
        // Si le joueur demande un indice via indic1 (indice généré par chatbot via GPT)
        else if (question == "indic1") {
            // Ajouter la demande d'indice dans l'historique des messages
            historiqueMessages.push_back({
                {"role", "user"},
                {"content", 
                  "Donne-moi un nouvel indice sur le mot secret. L'indice doit commencer par une phrase comme \"C'est un mot qui désigne...\" ou \"Ce mot est utilisé pour parler de...\". "
                  "Cette fois, l'indice doit parler de sa définition sans le révéler directement, proposer un synonyme possible ou décrire une situation typique où ce mot est utilisé. "
                  "Évite de répéter les anciens indices. C'est la " + std::to_string(nb_indice++) + "e fois que je demande. "
                  "IMPORTANT : Ne révèle jamais le mot secret. Ne le mentionne pas, même partiellement ou dans un exemple. Sois subtil, clair, mais jamais trop explicite."}
            });
        
            // Préparer la requête à l'API du chatbot
            json data;
            data["messages"] = historiqueMessages;
            data["temperature"] = 0.7;
            data["max_tokens"] = 150;
            data["model"] = "gpt-3.5-turbo";
        
            // Envoyer la question et recevoir la réponse brute
            std::string reponse_raw = bot.poserQuestion(data);
        
            try {
                // Analyser la réponse JSON
                json reponse_json = json::parse(reponse_raw);
                std::string indice = reponse_json["choices"][0]["message"]["content"];
                std::cout << "🔍 Indice: " << indice << std::endl;
        
                // Ajouter la réponse d'indice dans l'historique
                historiqueMessages.push_back({
                    {"role", "assistant"},
                    {"content", indice}
                });
        
            } catch (const std::exception& e) {
                std::cout << "Erreur lors du traitement JSON pour l'indice : " << e.what() << std::endl;
            }
        
            continue; // Revenir au début de la boucle pour une nouvelle question
        }
        // Si le joueur demande un indice via indic2 (indice venant d'une autre API)
        else if (question == "indic2") {
            std::string indice = obtenirIndice(mot_secret, indexIndice++);
            std::cout << "🔍 Indice: " << indice << std::endl;
            continue;
        }

        // Sinon, la question est ajoutée à l'historique comme question du joueur
        historiqueMessages.push_back({
            {"role", "user"},
            {"content", question}
        });

        // Préparer la requête avec l'historique complet pour le chatbot
        json data;
        data["messages"] = historiqueMessages;
        data["temperature"] = 0.7;
        data["max_tokens"] = 150;
        data["model"] = "gpt-3.5-turbo";

        // Envoyer la question au chatbot et récupérer la réponse brute
        std::string reponse_raw = bot.poserQuestion(data);

        try {
            // Parser la réponse JSON
            json reponse_json = json::parse(reponse_raw);

            if (reponse_json.contains("error")) {
                // Afficher une erreur si le chatbot renvoie une erreur
                std::cout << "❌ Erreur: " << reponse_json["error"]["message"] << std::endl;
            } else {
                // Extraire la réponse textuelle du chatbot et l'afficher
                std::string reponse_chatbot = reponse_json["choices"][0]["message"]["content"];
                std::cout << "Chatbot: " << reponse_chatbot << std::endl;
            
                // Ajouter la réponse du chatbot dans l'historique
                historiqueMessages.push_back({
                    {"role", "assistant"},
                    {"content", reponse_chatbot}
                });
            }

        } catch (const std::exception& e) {
            // Gérer une exception sur le parsing JSON
            std::cout << "Erreur lors du traitement de la réponse JSON : " << e.what() << std::endl;
            std::cout << "Réponse brute : " << reponse_raw << std::endl;
        }
    }
}

```
###### Résumé global :
- Le chatbot choisit un mot secret.
- Le joueur pose des questions pour deviner ce mot.
- Le chatbot répond par "oui" ou "non" grâce à l’API GPT.
- Le joueur peut demander des indices via indic1 (indice généré par GPT) ou indic2 (indice d’une autre API).
- Le jeu continue jusqu’à ce que le joueur trouve le mot ou abandonne.

#### Mode 2: Le chatbot devine
 ```text
Bienvenue dans le jeu de devinettes !
Choisissez le mode de jeu : 
1. Le joueur devine le mot du chatbot (Mode 1)
2. Le chatbot devine le mot du joueur (Mode 2)
Entrez 1 ou 2 : 1
Le mot secret a été choisi. Essayez de le deviner !
--- Mode 1 : Le chatbot pense à un mot ---
Posez vos questions (tapez mot que vous avez choisit ou 'echec' pour arrêter, 'indic1' pour obtenir une aide, indic2 pour obtenir une aide en anglais) :
(Exemples : Est-ce que c’est un animal ? / Est-ce que c’est quelque chose que je peux manger ?)
> 

Entrez 1 ou 2 : 2
--- Mode 2 : C'est le chatbot qui devine ---
Pensez à un mot. Le chatbot va essayer de le deviner.
(Pour tester : entrez votre mot secret ici, ne pas le montrer au chatbot) > 
```
----
```cpp
void Devinette::modeChatbotDevine() {
    // Afficher le mode de jeu choisi
    std::cout << "--- Mode 2 : C'est le chatbot qui devine ---" << std::endl;
    std::cout << "Pensez à un mot. Le chatbot va essayer de le deviner." << std::endl;

    // Demander à l'utilisateur de saisir son mot secret (à garder secret du chatbot)
    std::string mot_secret;
    std::cout << "(Pour tester : entrez votre mot secret ici, ne pas le montrer au chatbot) > ";
    std::cin >> mot_secret;

    std::string reponse;  // Pour stocker la réponse utilisateur (oui/non)
    std::string questions; // Pour garder l'historique des questions posées

    // Boucle principale où le chatbot pose des questions
    while (true) {
        // Préparer les messages à envoyer à l'API du chatbot
        json data;
        data["messages"] = {
            {
                {"role", "system"},
                {"content",
                 // Instructions détaillées données au chatbot pour guider son comportement
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
                // Envoyer la première demande ou les questions posées précédemment
                {"content", questions.empty() ? "Devine le mot." : questions}
            }
        };

        // Paramètres pour le modèle GPT
        data["temperature"] = 0.7;
        data["max_tokens"] = 100;
        data["model"] = "gpt-3.5-turbo";

        // Appeler l'API GPT pour générer la prochaine question du chatbot
        std::string reponse_raw = bot.poserQuestion(data);

        try {
            // Parse la réponse JSON
            json reponse_json = json::parse(reponse_raw);

            // Gestion d'erreur renvoyée par l'API
            if (reponse_json.contains("error")) {
                std::cout << "❌ Erreur: " << reponse_json["error"]["message"] << std::endl;
                break;
            }

            // Extraire la question proposée par le chatbot
            std::string proposition = reponse_json["choices"][0]["message"]["content"];
            std::cout << "Chatbot : " << proposition << std::endl;

            // Vérifier si la question a déjà été posée (éviter répétition)
            if (questions.find(proposition) != std::string::npos) {
                std::cout << "❌ Le chatbot a répété une question déjà posée. Fin du jeu." << std::endl;
                break;
            }

            // Demander à l'utilisateur de répondre à la question posée par le chatbot
            std::cout << "Répondez (oui/non) : ";
            std::cin >> reponse;

            // Vérifier si la réponse est valide
            if (reponse == "oui" || reponse == "non") {
                // Ajouter la question et la réponse à l'historique
                questions += proposition + ": " + reponse + "\n";

                // Si la question contient le mot secret ET que la réponse est "oui"
                // (ce cas est une sécurité mais normalement interdit par les règles)
                if (proposition.find(mot_secret) != std::string::npos && reponse == "oui") {
                    std::cout << "🎉 Le chatbot a trouvé ! C'était '" << mot_secret << "' !" << std::endl;
                    break;
                }
            } else {
                // Réponse invalide, demander à nouveau
                std::cout << "❌ Réponse invalide. Essayez 'oui' ou 'non'." << std::endl;
            }

        } catch (const std::exception& e) {
            // Gérer les erreurs lors du parsing JSON
            std::cout << "❌ Erreur lors du traitement de la réponse JSON : " << e.what() << std::endl;
            std::cout << "ℹ️ Réponse brute : " << reponse_raw << std::endl;
            break;
        }
    }
}

```
### Résultats obtenue:
- Si on gagane, tapez le mot secret
```text
 Bravo ! Vous avez deviné !
 Souhaitez-vous rejouer ? (o/n) o
```
- Si on échoue, tapez echec
```text
> echec
 Désolé ! À la prochaine fois !
 Le mot secret était : nourriture
 Souhaitez-vous rejouer ? (o/n) 
```
- Si on a besoin de indication, tapez indic1 ( le chatbot vous donne l'indication) , tapez indic2 (l'indication de tags et categories, API d'une site de dictionnaire)
##### Explications clés :
- Instructions détaillées au chatbot : lui interdisent de deviner ou dire directement le mot, et l'obligent à poser uniquement des questions fermées et non répétées.
- Historique des questions/réponses (questions) est gardé pour que le chatbot évite les répétitions et puisse s'appuyer sur le contexte.
- Interaction avec l'utilisateur : chaque question générée est affichée, puis l’utilisateur répond par "oui" ou "non".
- Arrêt du jeu : si le chatbot répète une question, ou si une erreur JSON survient, ou si le chatbot détecte (par hasard) le mot secret.

## 6. Test 
Il y a fichier de test:
- test1.cpp : les tests pour Chatbot
- test2.cpp : les tests pour Devinette 

```text
> ./text 1 
```
ou 
```text
> ./text 2 
```
#### Le Test 1
```cpp
// Test unitaire pour le Chatbot
// Ce test vérifie le bon fonctionnement de la classe Chatbot
#include "../src/Chatbot.hpp"        // Inclusion de la classe Chatbot à tester
#include <iostream>                  // Pour les entrées/sorties console
#include <nlohmann/json.hpp>         // Pour manipuler les données JSON facilement

using json = nlohmann::json;        // Alias pour simplifier l'utilisation de JSON

// Fonction pour afficher le résultat d'un test
void afficherResultat(const std::string& nomTest, bool succes) {
    // Affiche ✅ si succès, ❌ sinon, suivi du nom du test
    std::cout << (succes ? "✅ " : "❌ ") << nomTest << std::endl;
}

// Fonction qui vérifie si la réponse JSON est valide (contient "choices")
bool verifierReponseValide(const std::string& reponseBrute) {
    try {
        // On parse la chaîne brute en objet JSON
        json parsed = json::parse(reponseBrute);

        // On vérifie que la clé "choices" existe et n'est pas vide
        if (parsed.contains("choices") && !parsed["choices"].empty()) {
            // Extraction du contenu de la première réponse du chatbot
            std::string contenu = parsed["choices"][0]["message"]["content"];

            // Affiche le contenu extrait (utile pour debug)
            std::cout << "   Contenu extrait : " << contenu << std::endl;

            return true;  // Réponse JSON valide
        }
    } catch (...) {
        // En cas d'erreur (parse JSON échoué), retourne false
        return false;
    }
    return false;  // Si "choices" absent ou vide, retourne false
}

int main() {
    // URL de l'API LLM (à changer selon ton serveur)
    std::string api_url = "http://llmcode.math.unistra.fr:8090/v1/chat/completions";

    // Instanciation du chatbot avec l'URL de l'API
    Chatbot bot(api_url);

    // === Test 1 : Question simple basique ===
    {
        json data;
        data["model"] = "gpt-3.5-turbo";          // Modèle utilisé
        data["temperature"] = 0.7;                 // Température : créativité modérée
        data["max_tokens"] = 50;                    // Nombre max de tokens dans la réponse
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
        std::string response = bot.poserQuestion(data);  // Envoi la requête à l'API
        bool ok = verifierReponseValide(response);       // Vérifie si la réponse JSON est valide
        afficherResultat("Test 1", ok);                   // Affiche le résultat du test
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
        data["temperature"] = 0.0;                 // Température zéro : réponse la plus déterministe possible
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
        data["max_tokens"] = 5;                  // Très peu de tokens, la réponse risque d'être incomplète
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
                {"content", ""}                    // Message vide envoyé à l'assistant
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
                {"content", "Je suis là pour aider."}   // Message uniquement du rôle assistant (peu courant)
            }
        };

        std::cout << "=== Test 10 : Message avec rôle assistant uniquement ===\n";
        std::string response = bot.poserQuestion(data);
        bool ok = verifierReponseValide(response);
        afficherResultat("Test 10", ok);
        std::cout << std::endl;
    }

    return 0;   // Fin du programme
}
```
###### Résumé
- Inclusions et préparation :
  Le programme utilise la bibliothèque JSON (nlohmann/json) pour créer et manipuler facilement les requêtes JSON. Il inclut aussi la classe Chatbot pour interagir avec l'API.
- Fonction afficherResultat :
  Affiche un symbole ✅ ou ❌ selon si un test a réussi ou échoué, avec le nom du test.
- Fonction verifierReponseValide :
  Vérifie que la réponse reçue est un JSON valide et contient la structure attendue, notamment un tableau "choices" avec au moins une réponse. Elle extrait aussi le contenu de la réponse pour affichage.
- Tests dans main() :
  Chaque test construit un objet JSON avec différents paramètres (modèle, température, messages, max tokens), envoie cette requête via la méthode poserQuestion de Chatbot, puis vérifie la validité de la réponse.

#### Le Test 2
```cpp
#define CATCH_CONFIG_MAIN  // Génère automatiquement la fonction main() pour les tests Catch2
#include <catch2/catch_test_macros.hpp>  // Inclut les macros de test de Catch2
#include "../src/Devinette.hpp"  // Inclut la classe Devinette à tester


TEST_CASE("Test choisirMotAleatoire", "[choisirMotAleatoire]") {  // Début d’un test nommé "Test choisirMotAleatoire"
    Devinette dev;  // Création d’une instance de la classe Devinette
    std::string mot = dev.choisirMotAleatoire();  // Appelle la méthode pour choisir un mot aléatoire
    
    REQUIRE(!mot.empty());  // Vérifie que le mot choisi n’est pas vide
    REQUIRE(mot.length() >= 2);  // Vérifie que le mot a au moins 2 caractères
}

TEST_CASE("Test obtenirIndice avec mot réel", "[obtenirIndice]") {  // Test de la méthode obtenirIndice avec un mot existant
    Devinette dev;  // Instance de Devinette
    std::string mot_test = "chat";  // Mot de test fixe
    std::string indice = dev.obtenirIndice(mot_test, 0);  // Récupère l’indice du mot à la position 0

    REQUIRE_FALSE(indice.empty());  // Vérifie que l’indice n’est pas vide
    REQUIRE(indice.find("❌") == std::string::npos);  // Vérifie que l’indice ne contient pas le symbole d’erreur "❌"
}

TEST_CASE("Test obtenirIndice avec mot inexistant", "[obtenirIndice]") {  // Test obtenirIndice avec un mot qui n’existe pas
    Devinette dev;  // Instance de Devinette
    std::string mot_invalide = "zzzxxyyzzt";  // Mot invalide
    std::string indice = dev.obtenirIndice(mot_invalide, 0);  // Appelle obtenirIndice

    REQUIRE(indice.find("❌") != std::string::npos);  // Vérifie que l’indice contient "❌" pour signaler l’erreur
}

TEST_CASE("choisirMotAleatoire retourne des mots différents", "[choisirMotAleatoire]") {  // Test pour voir si choisirMotAleatoire retourne différents mots
    Devinette dev;  // Instance Devinette
    std::string mot1 = dev.choisirMotAleatoire();  // Premier mot choisi
    std::string mot2 = dev.choisirMotAleatoire();  // Deuxième mot choisi
    REQUIRE(!mot1.empty());  // Vérifie que le premier mot n’est pas vide
    REQUIRE(!mot2.empty());  // Vérifie que le deuxième mot n’est pas vide
    // Note : Les mots peuvent être identiques par hasard, on ne teste pas la différence stricte ici
}

// Test supplémentaire : vérifier que obtenirIndice retourne une chaîne de la bonne longueur
TEST_CASE("obtenirIndice retourne une chaîne de la bonne longueur", "[obtenirIndice]") {
    Devinette dev;  // Instance Devinette
    std::string mot = "chien";  // Mot test
    std::string indice = dev.obtenirIndice(mot, 0);  // Récupérer indice pour index 0

    REQUIRE(indice.length() == mot.length());  // L’indice doit avoir la même longueur que le mot
}

// Test supplémentaire : obtenirIndice avec index différent de 0
TEST_CASE("obtenirIndice avec index non nul", "[obtenirIndice]") {
    Devinette dev;  // Instance Devinette
    std::string mot = "chien";  // Mot test
    int index = 2;  // Index différent de 0
    std::string indice = dev.obtenirIndice(mot, index);  // Obtenir indice à l’index 2

    REQUIRE(indice.length() == mot.length());  // Vérifie que la longueur est toujours correcte
}

// Test supplémentaire : vérifier que lancer() ne provoque pas d’exception
TEST_CASE("lancer() s'exécute sans lancer d'exception", "[lancer]") {
    Devinette dev;  // Instance Devinette
    REQUIRE_NOTHROW(dev.lancer());  // Vérifie que la méthode lancer() s’exécute sans exception
}
```
#### Résumé
- Test choisirMotAleatoire
  Vérifie que la méthode retourne un mot non vide et d’au moins 2 caractères. Cela garantit que ta méthode génère des mots valides.
- Test obtenirIndice avec mot réel
  Vérifie que l’indice retourné pour un mot valide (ex : "chat") n’est pas vide et ne contient pas de marque d’erreur "❌".
- Test obtenirIndice avec mot inexistant
  Vérifie que si tu passes un mot qui n’existe pas dans ta base, ta méthode renvoie bien une erreur (signalée par "❌").
- Test choisirMotAleatoire retourne des mots différents
  Appelle deux fois la fonction pour s’assurer qu’elle peut retourner plusieurs mots valides, même si par hasard ils peuvent être identiques.
- Tests supplémentaires obtenirIndice
  Vérifie que la chaîne retournée a bien la même longueur que le mot, et ce pour différents index.
- Test lancer()
  Vérifie que la méthode lancer() s’exécute sans lancer d’exception, ce qui est important pour la robustesse.

#### Remarque 
Bien que lors de l'exécution du code, dans le test 2, des fonctions comme choisirMotAleatoire et obtenirIndice soient soulignées en erreur, cela signifie que ces deux fonctions sont privées mais sont appelées comme si elles étaient publiques. C’est pourquoi ici, dans le fichier Devinette.hpp, j’ai ajouté #ifdef TEST_MODE
```cpp
#ifndef DEVINETTE_HPP                 // Protection contre inclusion multiple
#define DEVINETTE_HPP                 // Définition de la macro DEVINETTE_HPP

#include "Chatbot.hpp"               // Inclusion de la classe Chatbot
#include <string>                   // Inclusion de la bibliothèque string

class Devinette {
public:
    Devinette();                    // Constructeur public de la classe
    void lancer();                  // Méthode publique pour démarrer le jeu

private:  
     void modeJoueurDevine();      // Méthode privée : mode où le joueur devine
     void modeChatbotDevine();     // Méthode privée : mode où le bot devine
     Chatbot bot;                  // Attribut privé : instance du chatbot

#ifdef TEST_MODE                   // Si la macro TEST_MODE est définie (mode test)
public:                           // alors rendre public les méthodes suivantes
   std::string choisirMotAleatoire();                // Méthode choisissant un mot aléatoire
   std::string obtenirIndice(const std::string& mot_secret, int index);  // Méthode retournant un indice sur le mot secret
#else                            // Sinon (en compilation normale)
private:                         // ces méthodes restent privées
    std::string choisirMotAleatoire();                // Méthode privée choisissant un mot aléatoire
    std::string obtenirIndice(const std::string& mot_secret, int index);  // Méthode privée donnant un indice
#endif                           // Fin du #ifdef TEST_MODE
};

#endif // DEVINETTE_HPP       // Fin de la protection contre inclusion multiple

```
Ici, Partie privée de la classe :
- Deux méthodes privées qui correspondent aux modes du jeu.
- Un attribut bot de type Chatbot pour gérer l’interaction avec le chatbot.
Directive conditionnelle de compilation :
Si TEST_MODE est défini (en général lors des tests unitaires), alors les méthodes choisirMotAleatoire et obtenirIndice sont publiques pour pouvoir être testées directement.
Sinon, ces méthodes restent privées, comme prévu dans la conception normale de la classe.
Cela permet d’accéder aux fonctions privées uniquement dans le cadre des tests, sans modifier la logique du programme dans la version finale.

#### Résumé
Ce fichier définit la classe Devinette avec plusieurs méthodes privées, notamment choisirMotAleatoire et obtenirIndice, qui ne devraient pas être accessibles depuis l’extérieur. Cependant, grâce à la directive #ifdef TEST_MODE, on rend ces méthodes publiques uniquement lors de la compilation en mode test, ce qui permet aux tests unitaires d’y accéder sans changer la conception normale de la classe.
donc on peut continuer à excecuter les codes sans probleme.
