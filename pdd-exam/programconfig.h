#ifndef PDD_PROGRAM_CONFIG_H
#define PDD_PROGRAM_CONFIG_H

#include <string>

namespace pdd {

class Config {
private:
    Config()
    : largePromptText0( "Здравствуйте!\n" )
    , largePromptText1( "Здравствуйте!\n" )
    , prompt_picture_file("../pdd_resources/prompt.png")
    , localMode(false)
    , serverPort(2020)
    , armNumber(1)
    , databaseName("../pdd_resources/pdd.db")
    , abPathToImages("../pdd_resources/images/ab/")
    , cdPathToImages("../pdd_resources/images/cd/")

    , abImageArchiveName( "../pdd_resources/ab/image.dat" )
    , abDllMainName( "../pdd_resources/ab/text.dat" )
    , abDllCommentName( "../pdd_resources/ab/comment.dat" )

    , cdImageArchiveName( "../pdd_resources/cd/image.dat" )
    , cdDllMainName( "../pdd_resources/cd/text.dat" )
    , cdDllCommentName( "../pdd_resources/cd/comment.dat" )

    , unzipPassword( "holy fucking sheet!" )
    , succesedTaskImage( "../pdd_resources/done.jpg" )
    , failedTaskImage( "../pdd_resources/fail.jpg" )
    , cheat(false)
    , usePromptFromFile(false)
    , use_prompt_picture(true)
    , protocolVersion(0)
    , viewOnlyWrong(false)
    , useCompressedImages(true)
    , useSqliteDatabase(false) {

        largePromptText0.append("С целью проведения теоретического экзамена Вам предложен экзаменационный билет, состоящий из 20 (или 40) вопросов.\n");
        largePromptText0.append("На каждый вопрос предложено несколько вариантов ответов, один из которых правильный.\n");
        largePromptText0.append("\nДля начала экзамена ДВАЖДЫ ЩЕЛКНИТЕ ЛЕВОЙ КНОПКОЙ МЫШИ в любом месте экрана.\n\n");
        largePromptText0.append("На экране появятся 20 (или 40) вопросов. Для выбора вопроса используйте левую кнопку мыши\n");
        largePromptText0.append("Для выбора варианта ответа на вопрос щелкните левой кнопкой мыши на соответствующем элементе.\n");
        largePromptText0.append("Подтвердите выбор, нажав на кнопку \"ОТВЕТИТЬ\", либо выберете другой вариант ответа, либо выберете другой вопрос нажав на кнопку \"НАЗАД\", \"К ПЕРЕЧНЮ\" или \"ДАЛЬШЕ\".\n");
        largePromptText0.append("Вопросы, на которые даны ответы закрашиваются серым цветом.\n");
        largePromptText0.append("Для ответов на все вопросы билета Вам предоставляется 20 (или 30) минут. Оставшееся время отображается в правой верхней части экрана.\n");
        largePromptText0.append("Экзамен заканчивается либо при положительном результате, либо при отрицательном результате, либо по истечению указанного времени.\n");
        largePromptText0.append("\nВАЖНО! Отрицательным результатом считается результат, когда:\nдопущены 2 ошибки в одной строке вопросов или\nдопущены 3 ошибки в разных строках вопросов или\nдопущена одна ошибка при ответе на дополнительные вопросы.\n\n");
        largePromptText0.append("\nПосле завершения экзамена и ознакомления с результатами дважды щелкните левой кнопкой мыши в любом месте экрана.\n");
        largePromptText0.append("Желаем успеха!\n");

        largePromptText1.append("С целью проведения теоретического экзамена Вам предложен экзаменационный билет, состоящий из 20 (или 40) вопросов.\n");
        largePromptText1.append("На каждый вопрос предложено несколько вариантов ответов, один из которых правильный.\n");
        largePromptText1.append("\nДля начала экзамена ДВАЖДЫ ЩЕЛКНИТЕ ЛЕВОЙ КНОПКОЙ МЫШИ в любом месте экрана.\n\n");
        largePromptText1.append("На экране появятся 20 (или 40) вопросов. Для выбора вопроса используйте левую кнопку мыши\n");
        largePromptText1.append("Для выбора варианта ответа на вопрос щелкните левой кнопкой мыши на соответствующем элементе.\n");
        largePromptText1.append("Подтвердите выбор, нажав на кнопку \"ОТВЕТИТЬ\", либо выберете другой вариант ответа, либо выберете другой вопрос нажав на кнопку \"НАЗАД\", \"К ПЕРЕЧНЮ\" или \"ДАЛЬШЕ\".\n");
        largePromptText1.append("Вопросы, на которые даны ответы закрашиваются серым цветом.\n");
        largePromptText1.append("Для ответов на все вопросы билета Вам предоставляется 20 (или 30) минут. Оставшееся время отображается в правой верхней части экрана.\n");
        largePromptText1.append("Экзамен заканчивается либо при положительном результате, либо при отрицательном результате, либо по истечению указанного времени.\n");
        largePromptText1.append("\nВАЖНО! Отрицательным результатом считается результат, когда:\nдопущены 2 ошибки в одной строке вопросов или\nдопущены 3 ошибки в разных строках вопросов.\n\n");
        largePromptText1.append("\nПосле завершения экзамена и ознакомления с результатами дважды щелкните левой кнопкой мыши в любом месте экрана.\n");
        largePromptText1.append("Желаем успеха!\n");

    }

    std::string largePromptText0;
    std::string largePromptText1;

public:

    static Config& inst() {
        static Config instance;
        return instance;
    }

    std::string largePromptText() const;

    std::string prompt_picture_file; /// имя файла с картинкой заставкой

    bool localMode; /// Тест на локальном компьютере
    unsigned int serverPort;
    unsigned int armNumber;

    /// Имя общей (для всех категорий) базы данных вопросов
    std::string databaseName;
    std::string abPathToImages; /// Путь к несжатыми иллюстрациями
    std::string cdPathToImages; /// Путь к несжатыми иллюстрациями

    /// Категория AB (формат PE)
    std::string abImageArchiveName; /// Имя архива с иллюстрациями.
    std::string abDllMainName; /// Имя файла со структурой и содержимым заданий.
    std::string abDllCommentName; /// Имя файла с пояснениями к заданиям.

    /// Категория CD (формат PE)
    std::string cdImageArchiveName; /// Имя архива с иллюстрациями.
    std::string cdDllMainName; /// Имя файла со структурой и содержимым заданий.
    std::string cdDllCommentName; /// Имя файла с пояснениями к заданиям.

    std::string unzipPassword; ///Пароль для распаковки иллюстраций из архива
    std::string succesedTaskImage;
    std::string failedTaskImage;

    bool cheat;

    bool usePromptFromFile; /// использовать для заставки текст из файла
    bool use_prompt_picture; /// использовать для заставки картинку из файла

    unsigned int protocolVersion; /// Версия протокола обмена. 0 - по умолчанию.
    unsigned int extraOff; /// Признак отключения доп вопросов.

    bool viewOnlyWrong; /// Признак демонстрации только неправильно отвеченных вопросов после теста

    bool useCompressedImages; /// Признак использования изображений из архива
    bool useSqliteDatabase; /// Признак использования SQLite базы данных
};

}

#endif // PDD_PROGRAM_CONFIG_H

