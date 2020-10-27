# Сборка и обновление Базы Знаний
docker-compose exec engine InfEngineManager.pl --dl-update /dldata -v &&
# Копирование собранной Базы Знаний
docker-compose exec engine cp /engine/release/dldata/dldata.ie2 /dldata
