//
// Created by thukisdo on 12/12/2021.
//

#pragma once
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

namespace tscl {

  /**
   * @brief variable statique correspondant au début du programme
   *
   */
  static const inline std::chrono::high_resolution_clock::time_point program_start =
          std::chrono::high_resolution_clock::now();

  /**
   * @brief Enumeration de tout les types de timestamp disponible
   *
   */
  enum class timestamp_t {
    /**
     * @brief Renvoi un timestamp vide
     *
     */
    None,
    /**
     * @brief Renvoie le temps écoulé depuis le début du programme au format 0.0s
     *
     */
    Delta,
    /**
     * @brief Renvoi l'heure actuelle au format Heures:Minutes:Secondes
     *
     */
    Partial,
    /**
     * @brief Renvoie la date complete, au format  Jour-Mois-Année Heures:Minutes:Secondes
     *
     */
    Full
  };

  /**
   * @brief Fonction retournant un timestamp formatté
   *
   * @param tst Type de timestamp a utilisé
   * @return std::string La date formatté
   */
  std::string timestamp(timestamp_t tst);

  /**
   * @brief Classe utilitaire représentant un chronometre
   *
   */
  class Chrono {
  private:
    /**
     * @brief Point dans le temps depuis la derniere sauvegarde du temps ecoulé
     *
     */
    std::chrono::high_resolution_clock::time_point m_begin;

    /**
     * @brief Temps ecoulé actuellement sauvegarder
     *
     */
    std::chrono::duration<double> m_current_duration;

    /**
     * @brief Vrai si le chronometre est en pause
     *
     */
    bool m_paused;

  public:
    /**
     * @brief Constructeur par défaut
     *
     * Initialise les points dans le temps
     *
     */
    Chrono();

    /**
     * @brief relance le chrono si celui ci est en pause
     *
     * @return Chrono&
     */
    Chrono &resume();

    /**
     * @brief Met le chrono en pause
     *
     *
     * @return Chrono&
     */
    Chrono &pause();

    /**
     * @brief Réinitialise le chrono et le relance
     *
     * @return Chrono&
     */
    Chrono &restart();

    /**
     * @brief Retourne le temps ecoulé
     *
     * @return std::chrono::duration<double>
     */
    std::chrono::duration<double> get();

    /**
     * @brief Retourne le temps dans l'unité selectionné
     *
     * @tparam TUnit Unité de temps
     * @return TUnit La quantité de temps écoulé dans la bonne unité
     */
    template<typename TUnit>
    TUnit getAs() {
      return std::chrono::duration_cast<TUnit>(get());
    }

    /**
     * @brief Insere la valeur du chrono dans un flux
     *
     * @param os Flux de sortie
     * @param val Le chrono a inserer
     * @return std::ostream& Le flux de sortie
     */
    friend std::ostream &operator<<(std::ostream &os, Chrono &val);

    /**
     * @brief Operateur de conversion vers un string, au format 0.0 (s)
     *
     * @return std::string La durée
     */
    operator std::string();
  };
}   // namespace tscl