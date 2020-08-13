docker-compose exec engine InfEngineManager.pl --dl-update /dldata -v &&
docker-compose exec engine cp /engine/release/dldata/dldata.ie2 /dldata
