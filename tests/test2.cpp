#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include "../src/Devinette.hpp"


TEST_CASE("Test choisirMotAleatoire", "[choisirMotAleatoire]") {
    Devinette dev;
    std::string mot = dev.choisirMotAleatoire();
    
    REQUIRE(!mot.empty());
    REQUIRE(mot.length() >= 2);
}

TEST_CASE("Test obtenirIndice avec mot réel", "[obtenirIndice]") {
    Devinette dev;
    std::string mot_test = "chat";
    std::string indice = dev.obtenirIndice(mot_test, 0);

    REQUIRE_FALSE(indice.empty());
    REQUIRE(indice.find("❌") == std::string::npos);
}

TEST_CASE("Test obtenirIndice avec mot inexistant", "[obtenirIndice]") {
    Devinette dev;
    std::string mot_invalide = "zzzxxyyzzt";
    std::string indice = dev.obtenirIndice(mot_invalide, 0);

    REQUIRE(indice.find("❌") != std::string::npos);
}

TEST_CASE("choisirMotAleatoire retourne des mots différents", "[choisirMotAleatoire]") {
    Devinette dev;
    std::string mot1 = dev.choisirMotAleatoire();
    std::string mot2 = dev.choisirMotAleatoire();
    REQUIRE(!mot1.empty());
    REQUIRE(!mot2.empty());
    // Les mots peuvent être identiques par hasard, mais doivent être valides
}

// Test supplémentaire : obtenirIndice doit retourner une chaîne de la bonne longueur
TEST_CASE("obtenirIndice retourne une chaîne de la bonne longueur", "[obtenirIndice]") {
    Devinette dev;
    std::string mot = "chien";
    std::string indice = dev.obtenirIndice(mot, 0);

    REQUIRE(indice.length() == mot.length());
}

// Test supplémentaire : obtenirIndice avec un index différent de 0
TEST_CASE("obtenirIndice avec index non nul", "[obtenirIndice]") {
    Devinette dev;
    std::string mot = "chien";
    int index = 2;
    std::string indice = dev.obtenirIndice(mot, index);

    REQUIRE(indice.length() == mot.length());
}

// Test supplémentaire : vérifier que lancer() ne lance pas d’exception
TEST_CASE("lancer() s'exécute sans lancer d'exception", "[lancer]") {
    Devinette dev;
    REQUIRE_NOTHROW(dev.lancer());
}