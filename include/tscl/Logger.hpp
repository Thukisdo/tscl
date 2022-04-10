/** Simple and efficient logging system created by Thukisdo (for personal use)
 *
 */

#pragma once


#pragma once

#include "Time.hpp"
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <unordered_map>

/**
 * @brief Namespace contenant le systeme de logs
 *
 */
namespace tscl {

  namespace errors {
    enum error_code {

      ERR_NONE = 0,
      ERR_ALLOCATION_FAILURE,
      ERR_ALREADY_EXISTING_HANDLER,
      ERR_UNKNOWN_HANDLER
    };
  }

  struct LoggerConfig;

  // ==================================================================
  // ===                         Basic Logs                         ===
  // ==================================================================

  /**
   * @brief Classe mère de tout les autres logs
   *
   */
  class Log {
  public:
    /**
     * @brief Enumération de tout les niveaux de log
     * Les logs critiques entraineront la fin du programme
     *
     */
    enum log_level { Trace, Debug, Information, Warning, Error, Fatal };

    /**
     * @brief Méthode permetant d'obtenir le niveau du log au format [niveau]
     *
     * @param level Niveau de log a formatté
     *
     * @return const std::string& Retourne une string contenant le niveau du log
     */
    static std::string const &levelToString(log_level level);

  private:
    /**
     * @brief niveau du log
     *
     */
    log_level m_level;

    /**
     * @brief Méthode abstraite permettant de récuperer le messages contenu dans le log
     *
     * @return std::string Contenu du log
     */
    virtual std::string messageImpl() const = 0;

  public:
    /**
     * @brief Constructeur par défaut du log
     *
     * @param level Permet de définir le niveau initial du log
     */
    Log(log_level level = Trace);

    /**
     * @brief Setter pour le niveau du log
     *
     * @param level Nouveau niveau du log
     */
    void level(log_level level);

    /**
     * @brief Getter pour le niveau du log
     *
     * @return log_level Niveau du log
     */
    log_level level() const;

    /**
     * @brief Getter pour le message du log, formatter pour comprendre
     * la timestamp si nécessaire, ainsi que le niveau
     *
     * @return std::string Le log formatter
     */
    virtual std::string prefix(tscl::timestamp_t ts_type) const;

    /**
     * @brief Getter pour le message du log, formatter pour comprendre
     * la timestamp si nécessaire, ainsi que le niveau
     *
     * @return std::string Le log formatter
     */
    virtual std::string message() const;
  };

  /**
   * @brief Log de base représentant un message sous forme de string
   *
   */
  class StringLog : public Log {
  protected:
    /**
     * @brief String contenant le message a transmettre
     *
     */
    std::string m_str;

    /**
     * @brief Surcharge de la méthode mére pour récuperer le contenu du message.
     *
     * @return std::string Renvoi le string
     */
    virtual std::string messageImpl() const override;

  public:
    /**
     * @brief Constructeur par défaut, définissant le niveau de log a Trace
     *
     * @param level
     */
    StringLog(log_level level = Log::Trace);

    /**
     * @brief Constructeur de base a partir d'un message, et d'un niveau optionnel
     *
     * @param message Message a transmettre
     * @param level Niveau du message
     */
    StringLog(std::string const &message, log_level level = Log::Trace);

    /**
     * @brief Constructeur de base a partir d'un message, utilise la sémantique de mouvement
     *
     * @param message Message a copier par mouvement
     */
    StringLog(std::string &&message, log_level level = Log::Trace);

    /**
     * @brief Constructeur de copie
     *
     * @param other Message a copier
     */
    StringLog(StringLog const &other) = default;

    /**
     * @brief Constructeur de mouvement
     *
     * @param other Message a copier par mouvement
     */
    StringLog(StringLog &&other);

    /**
     * @brief Assignation de copie par défaut
     *
     * @param other Message a copier
     * @return StringLog& La copie de l'objet
     */
    StringLog &operator=(StringLog const &other) = default;

    /**
     * @brief Assignation de mouvement
     *
     * @param other L'objet a copier par mouvement
     * @return StringLog& La copie de l'objet
     */
    StringLog &operator=(StringLog &&other);
  };

  // ==================================================================
  // ===                         Error Logs                         ===
  // ==================================================================

  /**
   * @brief Log représentant une erreur du programme.
   * Contient un code d'erreur, un message,
   * et une description optionelle
   *
   */
  class ErrorLog : public StringLog {
  protected:
    /**
     * @brief Description optionnelle de l'erreur
     *
     */
    std::string m_description;


    /**
     * @brief Identifiant unique de l'erreur qui est survenue
     *
     */
    long m_error_code;

    /**
     * @brief Méthode abstraite permettant de récuperer le messages
     * contenu dans le log
     *
     * @return std::string
     */
    virtual std::string messageImpl() const override;

  public:
    /**
     * @brief Constructeur de base pour une erreurs, prenant une erreur, un code erreur, ainsi
     * qu'une description optionelle
     *
     * @param error Message correspondant de l'erreurs
     * @param code L'identifiant unique de l'erreur
     * @param level Niveau de l'erreur
     * @param description Une description optionelle de l'erreur
     */
    ErrorLog(std::string const &error, long code = errors::ERR_NONE,
             Log::log_level level = Log::Error, std::string const &description = "");

    /**
     * @brief Constructeur de copie par défaut
     *
     * @param other La valeur a copié
     */
    ErrorLog(ErrorLog const &other) = default;

    /**
     * @brief Constructeur de copie par mouvement
     *
     * @param other La valeur a copié par mouvement
     */
    ErrorLog(ErrorLog &&other);

    /**
     * @brief Assignation de copie par défaut
     *
     * @param other la valeur a copier
     * @return ErrorLog& L'objet aprés copie
     */
    ErrorLog &operator=(ErrorLog const &other) = default;

    /**
     * @brief Assignation de mouvement
     *
     * @param other la valeur a copier par mouvement
     * @return ErrorLog& L'objet aprés copie
     */
    ErrorLog &operator=(ErrorLog &&other);

    /**
     * @brief Fonction surchargé pour pouvoir afficher correctement l'erreurs, y compris le code
     * d'erreurs et la description
     *
     * @param ts_type le type de timestamp a utilisé
     * @return std::string le message correctement formaté
     */
    virtual std::string message() const override;

    long errorCode() { return m_error_code; }
  };

  class ExceptionLog : public ErrorLog, public std::runtime_error {
  public:
    virtual const char *what() const noexcept override { return m_str.c_str(); }

    /**
     * @brief Constructeur de base pour une erreurs, prenant une erreur, un code erreur, ainsi
     * qu'une description optionelle
     *
     * @param error Message correspondant de l'erreurs
     * @param code L'identifiant unique de l'erreur
     * @param level Niveau de l'erreur
     * @param description Une description optionelle de l'erreur
     */
    ExceptionLog(std::string const &error, int code = errors::ERR_NONE,
                 Log::log_level level = Log::Error, std::string const &description = "")
        : ErrorLog(error, code, level, description) , std::runtime_error("") {}
  };

  // ==================================================================
  // ===                         Log handling                       ===
  // ==================================================================

  /**
   * @brief Classe abstraite recevant les logs, et les affichant correctement
   *
   */
  class LogHandler {
  private:
    /**
     * @brief Status of the handler, true if the handler is active
     *
     */
    bool m_enabled;

    /**
     * @brief Minimum level for logs to be treated
     *
     * Logs under this level will not be handled by this handler
     *
     */
    Log::log_level m_min_level;

    /**
     * @brief The type of timestamp to use for this handler
     *
     */
    tscl::timestamp_t m_ts_type;

    /**
     * @brief Construct a new Log Handler object
     *
     * @param val
     */
    LogHandler(LogHandler const &val) = delete;


    /**
     * @brief
     *
     * @param val
     * @return LogHandler&
     */
    LogHandler &operator=(LogHandler const &val) = delete;

  protected:
    /**
     * @brief Mutex permetant le logging depuis plusieurs thread en simultanés
     *
     */
    std::shared_mutex m_main_mutex;

  public:
    /**
     * @brief Constructeur du log handler, initialisant correctement la config
     *
     * @param config
     */
    LogHandler(bool enable = true, Log::log_level min_level = Log::Trace);

    virtual ~LogHandler() = default;

    /**
     * @brief Methode principale de logging
     *
     * @param log
     */
    virtual void log(Log const &log, std::string const &message) = 0;

    /**
     * @brief Active ou desactive ce handler
     *
     * @param val Vrai ou faux, dependamment si il faut activer ou désactiver ce handler
     */
    void enable(bool val) { m_enabled = val; }

    /**
     * @brief Getter retournant le statut de ce handler
     *
     * @return true Si ce handler est actif
     * @return false Sinon
     */
    bool enable() const { return m_enabled; }

    /**
     * @brief Setter pour définir le niveau minimum des logs a traité
     *
     * @param val
     */
    void minLvl(Log::log_level val) { m_min_level = val; }

    /**
     * @brief Getter pour obtenir le niveau minimum des logs traité
     *
     * @return Log::log_level
     */
    Log::log_level minLvl() const { return m_min_level; }

    /**
     * @brief Setter pour définir le style de timestamp a utilisé
     *
     * @param ts_type
     */
    void tsType(timestamp_t ts_type) { m_ts_type = ts_type; }

    /**
     * @brief Getter pour obtenir le style de timestamp utilisé
     *
     * @return util::time::timestamp_t Le style de timestamp a utilisé
     */
    timestamp_t tsType() const { return m_ts_type; }
  };

  /**
   * @brief Spécialisation de LogHandler pour prendre en charge la sortie vers le terminal
   *
   */
  class StreamLogHandler : public LogHandler {
  private:
    /**
     * @brief Méthode utilitaire pour colorer les messages
     *
     * @param level
     * @return std::string
     */
    std::string colorize(Log::log_level level);

    /**
     * @brief Pointeur sur le stream de sortie
     *
     * Peut pointer vers un stream extérieur, ou vers un stream alloué localement
     *
     */
    std::ostream *m_out;

    /**
     * @brief Booléen valant vrai si le stream a été alloué localement
     *
     */
    bool m_stream_owner;

    /**
     * @brief Booléen valant vrai si le handler doit afficher des couleurs a l'aide de code ascii
     *
     */
    bool m_use_ascii_color;

  public:
    /**
     * @brief Construit un Handler a partir d'un stream déja existant
     *
     * @param out Stream dans lequel les messages seront envoyés
     */
    StreamLogHandler(std::ostream &out, bool use_ascii_colors = true);

    /**
     * @brief Construit un Handler vers un fichier a partir d'un path
     *
     * @param path Chemin du fichier a ouvrir
     */
    StreamLogHandler(std::string const &path);

    /**
     * @brief Désalloue la mémoire si l'objet est propriétaire du stream
     *
     */
    ~StreamLogHandler();

    /**
     * @brief Methode principal de logs, utilisera les couleurs si possible
     *
     * @param log
     */
    virtual void log(Log const &log, std::string const &message);

    /**
     * @brief Setter permettant de définir l'utilisation des codes couleurs ascii
     *
     * @param val
     */
    void useAsciiColors(bool val) {
      std::unique_lock<std::shared_mutex> lock(m_main_mutex);
      m_use_ascii_color = val;
    }

    /**
     * @brief Getter permettant d'obtenir l'utilisation des codes couleurs ascii
     *
     * @return true Si le handler utilise des ccode ascii pour les couleurs
     * @return false Si le handler n'affiche pas de couleurs
     */
    bool useAscciColors() {
      std::shared_lock<std::shared_mutex> lock(m_main_mutex);
      return m_use_ascii_color;
    }
  };

  // ==================================================================
  // ===                         Logger                             ===
  // ==================================================================

  /**
   * @brief Singleton responsable de la bonne gestion des logs
   *
   */
  class Logger {
  private:
    /**
     * @brief dictionnaire contenant tout les loggers, identifié par une clé unique
     *
     */
    std::unordered_map<std::string, std::unique_ptr<LogHandler>> m_loggers;

    /**
     * @brief Constructeur privé pour maintenir l'état de singleton
     *
     */
    Logger(){};

    /**
     * @brief Suppression du constructeur de copie pour maintenir l'état de singleton
     *
     */
    Logger(Logger const &) = delete;

    /**
     * @brief Suppression du l'assignation de copie pour maintenir l'état de singleton
     *
     * @return Logger&
     */
    Logger &operator=(Logger const &) = delete;

    /**
     * @brief Mutex permetant le log simultanés depuis plusieurs threads
     *
     */
    std::shared_mutex m_main_mutex;

  public:
    /**
     * @brief Retourne le singleton
     *
     * @return Logger&
     */
    static Logger &singleton() {
      static Logger singleton;

      return singleton;
    }

    /**
     * @brief Fonction permettant d'envoyer un logs au different gestionnaires de logs
     *
     * @param log
     * @return Logger&
     */
    Logger &operator()(Log const &log) noexcept;

    /**
     * @brief Fonction permettant d'envoyer un message au different gestionnaires de logs
     *
     * @param msg
     * @param level
     * @return Logger&
     */
    Logger &operator()(std::string const &msg, Log::log_level level = Log::Trace) noexcept;

    /**
     * @brief Méthode permetant d'ajouter un gestionnaire au systeme de log
     *
     * @tparam THandler type du gestionnaire a rajouté
     * @tparam Args arguments variadiques
     * @param name Identifiant du gestionnaire
     * @param args Arguments a passé au constructeur du nouveau gestionnaire
     * @return LogHandler& Retourne une reference sur le nouveau gestionnaire
     */
    template<class THandler, typename... Args>
    THandler &addHandler(std::string name, Args &...args) noexcept {
      std::unique_lock<std::shared_mutex> lock(m_main_mutex);
      std::unique_ptr<LogHandler> buffer;

      try {
        buffer = std::make_unique<THandler>(args...);
      } catch (std::bad_alloc e) {
        operator()(ErrorLog("Failed log handler allocation", errors::ERR_ALLOCATION_FAILURE,
                            Log::Fatal,
                            "An allocation error occured during log handler allocation"));
      }

      auto tmp = m_loggers.emplace(name, std::move(buffer));

      lock.unlock();

      if (not tmp.second) {
        operator()(ErrorLog("Cannot add handler \"" + name +
                                    "\" : already existing handler with the same name",
                            errors::ERR_ALREADY_EXISTING_HANDLER, Log::Warning));
      } else
        operator()("Adding a new log handler : \"" + name + '\"');

      auto &res = *tmp.first->second;

      return dynamic_cast<THandler &>(res);
    }

    template<class THandler>
    THandler *getHandler(std::string const &name) noexcept {
      std::unique_lock<std::shared_mutex> lock(m_main_mutex);

      auto tmp = m_loggers.find(name);
      if (tmp == m_loggers.end()) {
        operator()(ErrorLog("Cannot return log handler \"" + name + "\"",
                            errors::ERR_UNKNOWN_HANDLER, Log::Error));
        return nullptr;
      }

      auto res = dynamic_cast<THandler *>(tmp->second.get());

      assert(res);

      return res;
    }

    /**
     * @brief Retire un gestionnaire de log du systeme
     *
     * @param name Nom du gestionnaire a retiré
     */
    void removeHandler(std::string name);
  };

  /**
   * @brief Objet global permettant de faciliter les opérations de logging
   *
   */
  extern Logger &logger;

}   // namespace tscl