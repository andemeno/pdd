#ifndef PDD_DATABOX_H
#define PDD_DATABOX_H

#include <map>
#include <vector>
#include <QObject>
#include <QPixmap>
#include <QTimer>
#include "question.h"

namespace pdd {

/**
 * Класс обеспечивает загруку ресурсов для вопросов
 * и доступ к ним. Синглтон.
 */
class DataBox : public QObject {
public:
    typedef std::vector<Question> Questions;
private:

    Q_OBJECT
    explicit DataBox(QObject *parent = 0);

    /**
     * Согласно Регламенту все вопросы (800 штук) разбиты на 4 группы по 200 в каждой.
     * Если вопросы расположить по темам с 1 по 28, то
     *  к 1ой группе будут относиться вопросы с 1 по 200,
     *  ко 2ой группе с 201 по 400,
     *  к 3ей группе с 401 по 600,
     *  к 4ой группе с 601 по 800.
     *
     * В каждой группе вопросы разбиты на тематические блоки по 5 вопросов
     * в порядке следования.
     *
     * Билет для экзамена формируется из 4ех тематических блоков,
     * каждый из которых выбран случайным образом из соответствующей группы.
     * Таким образом, в билете 20 вопросов.
     *
     * Дополнительный тематический блок выбирается из той же группы,
     * что и тематический блок, при ответе на вопросы которого была допущена ошибка или не дан ответ.
     */

    /**
     * Инициализация комплектов вопросов.
     * @return false если во время инициализации произошла ошибка.
     * @return true в случае успешной загрузки.
     */
    bool load();

    enum CategoryType {
        category_none,
        category_ab,
        category_cd
    };

    /// Структура заданий по каждой из категорий AB и CD:

    typedef std::vector<Questions> QuestionsGroup;
    typedef std::vector<QuestionsGroup> Groups;
    struct Category {
        Category();
        bool load_from_pe(const std::string& mainDll, const std::string& dllComment);
        bool load_from_sql(const std::string& dbName, const QString category_suffix);
        bool load(const std::string& mainDll, const std::string& dllComment);
        uint groupsCount; /// количество групп
        uint themeBlocksCount; /// количество тематических блоков
        uint questionsInThemeBlock; /// количество вопросов в тематическом блоке
        Groups groups;
        QStringList themes;
    };

    Category ab;
    Category cd;

    CategoryType categoryType; /// Тип комплекта вопросов текущего билета
    Questions questions; /// Вопросы текущего билета
    std::vector<int> answers; /// Ответы экзаменуемого
    std::map<uint, uint> errors; /// Номера тематических блоков, в которых допущены ошибки
    uint totalAnswers; /// Количество вопросов, на которые даны ответы
    uint totalExtraQuestions; /// Количество дополнительных вопросов
    std::vector<QPixmap> pixmaps; /// Иллюстрации вопросов текущего билета

    QTimer timer;

    class ImageNameFinder {
        QString toFind;
    public:
        ImageNameFinder( const QString& name ) : toFind(name) {}
        bool operator() (const Question& q ) {
            return q.getImageName() == toFind;
        }      
    };

    /** Извлечение из архива иллюстраций для заданного множества вопросов */
    std::vector<QPixmap> initPixmaps(Questions &quests);

    uint remainingTaskTime;
    uint elapsedTaskTime; /// Время, затраченное на выполнение текущего блока вопросов (оснвного или дополнительного)
    uint totalElapsedTaskTime; /// Время, затарченное на выполнение всего билета

signals:
    void taskStarted(); /// Сигнал "начался экзамен"
    void failTask(); /// Сигнал "экзамен НЕ СДАН"
    void successTask(); /// Сигнал "экзамен СДАН"
    void extraTask(uint count); /// Сигнал "необходимы доп.вопросы"

    /// Сигнал 2зарегистрирован ответ на вопрос"
    void questionAnswered(uint number, uint id, uint userAnswer, uint rightAnswer);

public:

    /**
     * Начало выполнения задания.
     * Необходимо вызывать для начала основного теста
     * и для начала задания с доп.блоком вопросов (если предусмотрено Регламентом).
     * Запуск таймера.
     */
    void startTask();

    /**
     * Иллюстрация для вопроса текущего билета с заданным номером
     * @param number номер вопроса в билете.
     */
    const QPixmap& getImage( const uint number ) const;

    /** Количество тем */
    uint themesCount( const bool categoryAB ) const;

    /** Список тем */
    const QStringList& getThemes(const bool categoryAB) const;

    /** Название темы с заданным номером */
    QString getTheme(const uint themeNumber, const bool categoryAB) const;

    /**
     * Формирование билета из заданного количества вопросов с соблюдением регламента
     * @param count количество вопросов в билете
     */
    void initCommonTask( const uint count, const bool categoryAB );

    /**
     * Формирование билета состоящего из вопросов заданной темы
     * @param themeNumber номер темы
     */
    void initByTheme( const uint themeNumber, const bool categoryAB );

    /** Вопросы текущего билета */
    const Questions& getQuestions() const;

    /**
     * Вопрос текущего билета с заданным номером
     * @param number номер вопроса
     */
    const Question getQuestion(const uint number) const;

    /**
     * Сравнение ответа экзаменуемого и правильного ответа
     * @param number номер вопроса из текущего блока
     * @return true если экзаменуемый дал правильный ответ
     * @return false если экзаменуемый дал неправильный ответ или не дал ответа вообще.
     */
    bool checkAnswer(const uint number) const;

    /**
     * Проверка, дал ли экзаменуемый неправильный ответ на этот вопрос.
     * @param number номер вопроса из текущего билета
     * @return true если был дан неправильный ответ
     * @return false если был дан правильный ответ или не было ответа
     */
    bool wrongAnswered(const uint number) const;

    /**
     * Номер ответа экзаменуемого на заданный вопрос.
     * @param number номер вопроса в билете.
     * @return -1 если экзаменуемый не ответил на вопрос.
     * @return > 0 номер ответа.
     */
    int getAnswer(const uint number) const;

    /** Количество дополнительных вопросов */
    uint getExtraCount() const;

    /** Время, оставшееся до конца экзамена */
    QTime getTaskRemainingTime();

    /** Время затраченное на экзамен */
    QTime getTaskElapsedTime() const;

    /** Экземпляр объекта */
    static DataBox& inst();

public slots:

    /**
     * Сохранение ответа на вопрос с заданным номером.
     * Проверка соответствия Регламенту.
     * Генерирует сигнал failTask(), если выявлено что экзамен не СДАН
     * Генерирует сигнал successTask() после ответа на последний вопрос билета, если экзамен СДАН
     * Генерирует сигнал extraTask() после ответа на последний вопрос билета, если необходим доп.блок вопросов.
     * @param number номер вопроса в текущем билете.
     * @param answer номер ответа экзаменуемого.
     */
    void setAnswer( const uint number, const uint answer );

    /**
     * Действия, выполняемые по истеченю времени, отведенного на билет или доп.блок вопросов.
     * Генерирует сигнал failTask(), если выявлено что экзамен не СДАН
     * Генерирует сигнал successTask() если экзамен СДАН
     * Генерирует сигнал extraTask() если необходим доп.блок вопросов.
     * Остановка таймера.
     */
    void onTimeoutTask();

    /**
     * Формирование дополнительного блока вопросов с соблюдением регламента
     * @return количество добавленных вопросов
     */
    void initExtraTask();

};

}

#endif // PDD_DATABOX_H
