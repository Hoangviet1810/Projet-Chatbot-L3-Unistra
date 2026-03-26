# chatbotl3
# Projet Devinette - Chatbot en C++

## Table des Matières

- [Contexte](#contexte)
  - [Mode de jeu](#mode-de-jeu)
- [Prérequis](#prérequis)
  - [Remarque](#remarque)
  - [Compilation et exécution](#compilation-et-exécution)
    - [Mode 1 : le joueur devine](#mode-1--le-joueur-devine)
    - [Mode 2 : le chatbot devine](#mode-2--le-chatbot-devine)
  - [Tests](#tests)
- [Structure](#structure)
- [Ajout de vos fichiers](#add-your-files)
- [Nom du projet](#name)
- [Description](#description)
- [Dépendances et gestion des bibliothèques externes](#dépendances-et-gestion-des-bibliothèques-externes)
- [Références](#réference)

## Contexte
Ce projet est réalisé dans le cadre de la Licence 3 Mathématiques Appliquées à l'Université de Strasbourg. 

Développement d’un chatbot de jeu de devinettes utilisant un modèle de langage large (LLM) distant, accessible via une API HTTP, implémenté en C++. Ce projet implémente un jeu de devinettes avec un chatbot utilisant C++17, CPR, JSON et Catch2 pour les tests.

Il y a 2 mode de joue:
- Mode 1 : Le joueur devine
- Mode 2 : Le chatbot devine

---

## Prérequis

- CMake version >= 3.10
- Compilateur C++ compatible C++17 (ex : g++, clang++)
- Git (pour cloner les dépendances via FetchContent)
- Connexion Internet (pour récupérer les dépendances la première fois)
- Connexion de VPN de l'Université de Strasbourg ( sinon on peut changer le link de d'autre chatbot IA (chatgpt, minsitral, deepseek, etc ) pour continuer.)

1, Créer un dossier build et se placer dedans :
```bash
mkdir build
cd build
cmake ..
make
```
#### Remarque
 - Lors du lancement du code avec la commande `make`, si l'exécution semble bloquée ou prend trop de temps, il est possible d'ouvrir un nouveau terminal et de relancer la commande `make`.  Cela peut parfois résoudre le problème et accélérer l'exécution du code.

2, Pour jouer le jeu de devinettes, il faut taper:
```bash
./go
```
2.1, Pour jouer le Mode 1: le jouer devine
```bash
1
```
et suivre les indications.

2.2, Pour jouer le Mode 2: le chatbot devine
```bash
2
```

3, Pour faire les tests, il faut taper:
```bash
./test1
```
pour faire les tests avec le file Chatbot.cpp

et taper:
```bash
./test2
```
pour faire les tests avec le file Devinette.cpp

# Structure
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
## Add your files
```
cd existing_repo
git remote add origin https://git.unistra.fr/dhvnguyen/chatbotl3.git
git branch -M main
git push -uf origin main
```
## Name
Chatbot Devinette — un chatbot intelligent en C++ conçu pour jouer à des jeux de devinettes interactifs avec l'utilisateur. 

## Description
Ce projet consiste en un chatbot développé en C++ qui permet de jouer à un jeu de devinettes. Le chatbot peut fonctionner en deux modes :

Mode 1 : Le joueur essaie de deviner le mot choisi aléatoirement par le programme.

Mode 2 : Le chatbot tente de deviner le mot choisi par le joueur grâce à des indices.

Le projet utilise plusieurs bibliothèques modernes, telles que Catch2 pour les tests unitaires, nlohmann/json pour la manipulation JSON, et cpr pour les requêtes HTTP. L’objectif principal est de créer un chatbot intelligent et interactif qui offre une expérience de jeu fluide et engageante.

## Dépendances et gestion des bibliothèques externes
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
# Réference
- Llama ("http://llmcode.math.unistra.fr:8090/v1/chat/completions")
- JSON (https://github.com/nlohmann/json.git)
- CPR (https://github.com/libcpr/cpr.git)
- Catch2 (https://github.com/catchorg/Catch2.git)
- Randoom (https://random-words-api.vercel.app/word/french)
- Chercher definition (https://api.arasaac.org/api/pictograms/fr/bestsearch/) / (https://dico.apiceras.ch/)
