# Вычисление корневой директории.
ROOTDIR=`dirname $0`

# Переход в корневую директорию.
cd ${ROOTDIR}/..

# Запуск контейнеров.
docker-compose up -d $@