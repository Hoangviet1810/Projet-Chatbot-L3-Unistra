#ifndef DEVINETTE_HPP
#define DEVINETTE_HPP

#include "Chatbot.hpp"
#include <string>

class Devinette {
public:
    Devinette();
    void lancer();

private:  
     void modeJoueurDevine();   // Mode 1: le joueur devine
     void modeChatbotDevine();  // Mode 2: le bot devine
     Chatbot bot;
#ifdef TEST_MODE
public: 
   std::string choisirMotAleatoire();
   std::string obtenirIndice(const std::string& mot_secret, int index);
#else   
private:
    std::string choisirMotAleatoire();
    std::string obtenirIndice(const std::string& mot_secret, int index);
#endif
};

#endif // DEVINETTE_HPP
