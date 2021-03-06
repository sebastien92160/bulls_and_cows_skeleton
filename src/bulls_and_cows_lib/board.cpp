
#include "board.hpp"
#include "random.hpp"
#include <string>
#include <map>

using namespace std; // lifehack

//pk utiliser des outpout_stream/input_stream passer en parametre  au lieu d'utiliser tout simplement des cout cin de base?
namespace bulls_and_cows {
    
    Board create_board(const GameOptions& game_options)
    {
        Board b; // créer une structure board vide
        for (size_t i = 0; i < game_options.number_of_characters_per_code; i++) // boucle for qui remplit le code secret d'une taille game_options.number_of_characters_per_code
        {
            b.secret_code.value += generate_random_character(
                game_options.minimum_allowed_character,
                game_options.maximum_allowed_character); // lettre entre la lettre min et max de l'option passé en parametre
        }
        return b;
    }

    bool validate_attempt(const GameOptions& game_options, const Code& attempt)
    {
        string codex = attempt.value; //tentative = flux d entrée = input = attempt.value
        if (codex.length() != game_options.number_of_characters_per_code) // verification de la taille du code
        {
            return false;
        }

        for (int i = 0; i < codex.size(); i++) //  on parcourt de chaque caracter du codex
        {
            if (codex[i] < game_options.minimum_allowed_character || codex[i] > game_options.maximum_allowed_character) // si une des lettres est en dehors des bornes fixés alors on retourne false// (genre 'A' et 'H')                                                                
            {
                return false;
            }
        }

        return true;
    }

    Feedback compare_attempt_with_secret_code(const Code& attempt, const Code& secret_code)
    {
        string REALCODE = secret_code.value;
        string codex = attempt.value;
        Feedback f;
        int cows = 0;
        int bulls = 0;

        //on crééer un tableau de boolean qui permetra de mettre en memoire les charactere en bulls. Ainsi, un bulls ne peut pas être un cows (voir ligne 70)
        vector<bool> already_used_secret_code_characters;
        for (int i = 0; i < codex.size(); i++)
        {
            already_used_secret_code_characters.push_back(false);
        }

        // on compte les bulls
        for (int i = 0; i < codex.size(); i++) // on parcourt de chaque caracter du codex
        {
            if (codex[i] == REALCODE[i]) // si on a trouvé le caractere dans le code
            {
                bulls++;
                already_used_secret_code_characters[i] = true; // on sauvegarde la "mémoire"
            }
        }

        // on compte les cows
        // !!!!!! on part du principe que si dans l'attempt on met 2 fois un char présent 1 fois dans le secret_code,
        // alors on a 2 cows. <- FAUX
        for (int i = 0; i < codex.size(); i++) // on parcourt de chaque caracter du codex
        {
            size_t found_realcode_char_index = REALCODE.find(codex[i]);
            if (found_realcode_char_index != string::npos && codex[i] != REALCODE[i] && !already_used_secret_code_characters[found_realcode_char_index] && found_realcode_char_index != i) // si on a trouvé le caractere dans le code et ce n'est pas un bulls alors ok
            {
                cows++;
                already_used_secret_code_characters[found_realcode_char_index] = true;//MODIFICATION Réalisée pour éviter d'avoir plusieurs cows sur le meme caractère du secret code 
            
             //AVANT MODIF
            // secret code :A B A C D
            // code rentré :A B F A A 
            // 1 Bull et 2 cows

            //APRES MODIF
                // secret code :A B A C D
                // code rentré :A B F A A
                // 1 Bull et 1 cow
            
            
            
            }
        }



        f.cows = cows;
        f.bulls = bulls;

        return f;
        

    }

    bool is_end_of_game(const GameOptions& game_options, const Board& board)
    {
        // attempt pareil que secretcode
        for (int i = 0; i < size(board.attempts_and_feedbacks);
             i++) // on parcourt la liste des attemptes et on regarde si il y en a un similaire au code secret
        {
            if (board.attempts_and_feedbacks[i].attempt.value == board.secret_code.value) // on aurait aussi put dire if (nbr de bulls == nbr de charactere)
            {
                return true;
            }
        }

        // derniere tentative
        if (size(board.attempts_and_feedbacks) >= game_options.max_number_of_attempts) // on aurait put mettre == au lieu de >= mais c est une sécurité
        {
            return true;
        }

        return false;
    }

    bool is_win(const GameOptions& game_options, const Board& board)
    {
        if (board.attempts_and_feedbacks.size() == 0) // on vérifie qu'il y a déjà eu au moins une tentative
        {
            return false;
        }
        if (board.attempts_and_feedbacks[board.attempts_and_feedbacks.size()-1].attempt.value == board.secret_code.value) // on regarde la derniere valeur du tableau : if ( last attempt = secret code)  (board.attempts_and_feedbacks.size()-1 = index de la derniere valeur du vecteur)
        {
            return true;
        }
        return false;
    }

    //TODO si j'ai  le temps: j'ai pas fait d'affichage dynamique en fonction de la longueur de l'attempt donc si long
    void display_board(std::ostream& output_stream, const GameOptions& game_options, const Board& board)
    {

            string showup =
                "-------------------------------------\n"
                "| SECRET   * * * * * |              |\n"
                "-------------------------------------\n"
                "| ATTEMPTS           | BULLS | COWS |\n"
                "-------------------------------------\n";

            // Attention!! Il faut prendre en compte les lignes pas encore remplis
            for (int i = game_options.max_number_of_attempts-1; i > ((int)board.attempts_and_feedbacks.size())-1; i--)
            {
                if (i>8)
                {
                    showup += "| #" + to_string(i + 1) + "      . . . . . |       |      |\n";
                }
                else
                {
                    showup += "| #0" + to_string(i + 1) + "      . . . . . |       |      |\n";
                }
               
            }
           
            for (int i = (int)board.attempts_and_feedbacks.size()-1; i >= 0; i--) //size_t est tjr positif (impossible de faire i-- quand i=0 car i devient 2^32) donc on trick
            {
                string numero;
                if ((i+1)>9)
                {
                    numero = "| #" + to_string(i + 1);
                }
                else
                {
                    numero = "| #0" + to_string(i + 1); //ici, si j utilise size_t ou unsigned int, si i<0, alors il y a undépassement de capacité
                }
                showup += numero + "          "+ board.attempts_and_feedbacks[i].attempt.value + " |   " +
                          to_string(board.attempts_and_feedbacks[i].feedback.bulls) + "   |   " +
                          to_string(board.attempts_and_feedbacks[i].feedback.cows) + "  |\n";
            }
            showup += "-------------------------------------\n";
            output_stream << showup; 
        //}
        
    }

    Code ask_attempt(std::ostream& output_stream, std::istream& input_stream, const GameOptions& game_options, const Board& board)
    {
        output_stream << "Rentrez votre tentative!" << endl;
        Code codex;
        input_stream >> codex.value;
        
        while (!validate_attempt(game_options, codex)) //on vérifie la qualitée de l'attempt ici
        {
            output_stream << "Rerentrez votre tentative, bien cette fois stp!" << endl;
            input_stream >> codex.value;
        }

        return codex;
    }

} // namespace bulls_and_cows
