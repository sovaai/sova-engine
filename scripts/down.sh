# Вычисление корневой директории.
ROOTDIR=`dirname $0`

# Переход в корневую директорию.
cd ${ROOTDIR}/..

# Остановка и удаление контейнеров.
docker-compose down $@