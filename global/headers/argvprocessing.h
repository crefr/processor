//! @file
#ifndef ARGVPR_INCLUDED
#define ARGVPR_INCLUDED

static const int MAXLIN = 100;
enum exits {ARGGOOD, ARGBAD, ARGEXIT};

//! @brief Структура, хранящая данные о флаге командной строки
typedef struct argvEx_t
{
    unsigned int valtype;       ///< Номер типа значения
    const char *fname;          ///< Название флага
    const char *help;           ///< То, что выведется в --help про этот флаг
} argvEx_t;

//! @brief Объединение, хранящее значение флага
typedef union fvals
{
    unsigned int bl : 1;  ///< Значение типа bool
    int in;               ///< Значение типа int
    double dbl;           ///< Значение типа double
    char str[MAXLIN];     ///< Строка
} fvals_t;

enum valtypes {V_NOTDEF = 0, V_BOOL, V_STRING, V_DOUBLE, V_INT};
enum flagsletters{H_flag, HELP_flag, I_flag, O_flag, T_flag};
const struct argvEx_t args[] =
{
    {.valtype = V_NOTDEF,       .fname = "-h",          .help = "standard --help"},
    {.valtype = V_NOTDEF,       .fname = "--help",      .help = "standard --help"},
    {.valtype = V_STRING,       .fname = "-i",          .help = "defines file to assemble (-i itest.asm)"},
    {.valtype = V_STRING,       .fname = "-o",          .help = "defines file for result of assembling (-o otest.bin)"}
};
const int ARGVNUM = sizeof(args) / sizeof(argvEx_t);

//! @brief Обрабатывает аргументы командной строки, примеры которых лежат в массиве структур argvEx_t в argvprocessing.cpp, заполняет массив fval значениями флагов
//! @param      [IN]    argc    argc из main, количество аргументов
//! @param      [IN]    argv    указатель на массив строк argv
//! @param      [OUT]   fval    массив типа union fvals, в который записываются значения флагов
//! @return     Возвращает BAD, если произошла какая-то ошибка, GOOD - если все хорошо (константы, заданы в consts.h)
int argvReceive(const int argc, char **argv, union fvals *fval);

//! @brief Заполняет значение флага fvals
//! @param      [IN]    argv        указатель на массив строк argv
//! @param      [IN]    argindex    индекс аргумента в argv
//! @param      [OUT]   fval        массив типа union fvals, в который записываются значения флагов
//! @param      [IN]    flagindex   индекс флага
//! @return     Возвращает BAD, если произошла какая-то ошибка, GOOD - если все хорошо (константы, заданы в consts.h)
int fillFval(char **argv, const int argindex, union fvals *fval, const int flagindex);

 //! @brief Печатает --help, берет данные из args, определенной в argvprocessing.cpp
void printHelp(void);


#endif
