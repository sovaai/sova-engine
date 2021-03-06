Диалоговое Ядро
===

Описание
===
Диалоговое Ядро - компонент Диалогового Процессора SOVA,предназначеного для использования в системах автоматизации диалога (ботах). Диалоговое ядро (engine), реализует функционал генерации текстовых ответов в реальном времени, используя для этого  бинарную поисковую базу, подготовленную компилятором языка DL.

Быстрый старт
===

Для запуска Диалогового Ядра в стандартном режиме необходимо выполнить следующую команду:
```bash
docker-compose up -d
```
Диалоговое ядро запустится с Базой Знаний по умолчанию в режиме docker-compose и будет принимать запросы на localhost:2255.

Для обновления Базы Знаний, необходимо положить исходные данные в директорию dldata (если ее нет, то она будет автоматичеки создана при запуске Диалогового Ядра) и выполнить команду:
```bash
./docker/update.sh
```
Запустится процесс компиляции Базы Знаний, причем бот Диалоговое Ядро будет работать и отвечать на запросы до самого момента замены скомпилированной Базы Знаний.
В случае, если в данных Базы Знаний были допущены ошибки, компиляци выдаст полное их описание и прервет обновление.
В случае, если компиляция прошла успешно, скомпилированная База Значний автоматически устанавливается в Диалоговое Ядро, а ее копия копируется в директорию /dldata под именем dldata.ie2 и может быть использована Диалоговым Ядром для быстрого старта с этой Базой Знаний.

Сборка Диалогового Ядра на Debian 10
===
1. Установка зависимостей:
  ```bash
  apt-get update && apt-get upgrade -y cmake \
      build-essential flex libgmp-dev \
      libmemcached-dev libreadline-dev libicu-dev
  ``` 
2. Сборка Диалогового Ядра:
  ```bash
  mkdir cmake_build && cd cmake_build && cmake ../build && make -j 5 
  ```

Компоненты Диалогового Ядра:
===
### Компилятор языка DL
Компилятор Языка DL – InfCompiler

##### Параметры командной строки
Получение подсказки поиспользованию:
```
InfCompiler --help
```
Вывод текущей версии компилятора и информации по сборке:
```
InfCompiler --version
```
Запуск компиляции:
```
InfCompiler [ConfigFile] [OPTIONS]
```
##### Конфигурационный файл
```
// Конфигурация компилятора языка DL.

[Main]
// Корневая директория для Диалогового Ядра. Значение по умолчанию - родительская диретория конфигурационного файла.
// RootDir = /usr/local/InfEngine

[Functions]
// Директория с библиотеками внешних функций.
RootDir = release/lib/functions

// Список внешних функций.
ConfigFile = conf/functions.lst


[Log]
// Уровень логирования. Возможные значения: [ "NONE", "ERROR", "WARN", "INFO", "DEBUG" ]. Значение по умолчанию: "NONE".
Level = INFO

// Идентификатор логирования. По умолчанию: "InfEngine Compiler".
Identificator = InfEngine Compiler


[Swap]
// Swap файл для компиляции.
FilePath = tmp/swap

// Ограничение памяти. Значение по умолчанию: "0" ( Swap отключен ).
MemoryLimit = 10000000


[InfCompiler]
// Корневой каталог для DL данных.
DLDataDir = data/dlstable

// Путь к списку файлов с шаблонами на языке DL.
DLPatternsList = dl.lst

// Путь к списку файлов со словарями на языке DL.
DLDictsList = dict.lst

// Путь к списку переменных, используемых в Диалоговом Ядре.
DLVarsList = defvars.lst

// Путь к целевой базе.
TargetPath = dldata.ie2

// Флаг жесткости компиляции. Возможные значения: [ "TRUE", "FALSE" ]. Значения по умолчанию: "TRUE"
StrictMode = true

// Путь к файлу с алиасами на языке DL.
DLAliases = aliases.dl

// Путь к списку файлов с синонимами.
SynonymsConfFile = syn.lst

// Корневой путь для файлов с синонимами.
SynonymsRootDir = synonyms

```

### Сервис Диалогового Ядра
Сервис Диалогового Ядра - InfServer

##### Параметры командной строки
Получение подсказки поиспользованию:
```
InfServer --help
```
Вывод текущей версии компилятора и информации по сборке:
```
InfServer --version
```
##### Конфигурационный файл
```
// Конфигурация Диалогового Ядра.

[Functions]
// Директория с библиотеками внешних функций.
RootDir = release/lib/functions

// Список внешних функций.
ConfigFile = conf/functions.lst


[Log]
// Уровень логирования. Возможные значения: [ "NONE", "ERROR", "WARN", "INFO", "DEBUG" ]. Значение по умолчанию: "NONE".
Level = WARN

// Идентификатор логирования. По умолчанию: "InfEngine Server".
Identificator = InfEngine Server

// Flag for logging all data flow between server and clients. Default value: "false".
// Флаг для логгирования всех данных проходящих через Диалоговое Ядро.
DataFlow = false


[Cache]
// Сервер кэширования.
Servers = localhost:11211

// TTL для кэш записей. Значение по умолчанию: "604800" ( одна неделя ).
TTL = 604800


[InfServer]
// Путь к бинарной Базе Знаний.
BasePath = release/dldata/dldata.ie2

// Максимальное число запросов, обрабатываемых одним процессом. Значение по умолчанию: "10000".
MaxRequestsNumber = 1000

// Таймаут для запроса. Значение по умолчанию: "30".
TimeOut = 30

// Путь к файлу с алиасами на языке DL.
DLAliases = release/dldata/aliases.dl

// Флаг отладочного режима.
DebugMode = YES
```

### Процесс Сервер
Процесс Сервер - Сервис реализующий запуск Сервисов Диалогового Ядра в мультипроцессном режиме

##### Параметры командной строки
```
ap-process-server path-to-inf-server
```

##### Кофигурационный файл
```
# Путь к InfServer.
FilterPath ./bin/InfServer

# Параметры запуска InfServer.
FilterParam ./conf/InfServer.conf

# Сокет, который процесс-сервер будет слушать.
Listen tcp::2255

# Логгирование.
LogLevel 0
SyslogFacility local3

# Путь к файлу с pid'ом процесс сервера.
PidFile ./tmp/process-server.pid

# Максимальное количество одновременных процессов InfServer.
MaxFilters 1
# Количество процессов InfServer, которые будут запущены при старте процесс-сервера.
StartFilters 0
# Минимальное разрешенное количество запущенных процессов InfServer.
MinSpareFilters 0

```

База Знаний
===
Пример Базы Знаний и формата всех необходимых для компиляции файлов представлен в директории /docker/dldata.