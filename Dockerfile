# Сборка базового образа для Debian 10.
FROM debian:10-slim as engine-base-debian-10

ADD docker/deb/libinfengine-common-perl_0.1.6-1_all.deb /deb/
ADD docker/deb/libinfengine-server-perl_0.2.3-1_all.deb /deb/

RUN apt-get update && \
	apt-get upgrade -y \
						gdebi \
						cmake \
						build-essential \
						flex \
						libgmp-dev \
						libmemcached-dev \
						libreadline-dev \
						libicu-dev \
						rsyslog && \
	gdebi /deb/libinfengine-common-perl_0.1.6-1_all.deb -n && \
	gdebi /deb/libinfengine-server-perl_0.2.3-1_all.deb -n && \
	apt-get clean && \
	rm -rf *.deb \
		/var/cache/apt/archives/* \
		/var/lib/apt/lists/* && \
	truncate /var/log/dpkg.log --size 0

# Сборка Компилятора DL.
FROM engine-base-debian-10 as engine-compiler-10

WORKDIR /src
ADD docker/config /engine
ADD . .

RUN ln -s /engine /usr/local/InfEngine && \
    InfEngineManager.pl --build -j 5 --verbose && \
    InfEngineManager.pl --install --trust --verbose && \
    InfEngineManager.pl --dl-update /src/docker/dldata

# Сборка Диалогового Ядра.
FROM engine-base-debian-10 as engine-server

COPY --from=engine-compiler-10 /engine /engine
COPY --from=engine-compiler-10 /src/docker/start.sh /scripts/

RUN ln -s /engine /usr/local/InfEngine && \
    sed -i '/imklog/s/^/#/' /etc/rsyslog.conf

CMD /scripts/start.sh || cat /usr/local/InfEngine/logs/InfEngine.log
