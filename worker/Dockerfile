FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Moscow

RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake \
    build-essential \
    git \
    wget \
    libqt6core6 \
    libqt6network6 \
    libqt6httpserver6 \
    libqt6xml6 \
    qt6-base-dev \
    qt6-httpserver-dev \
    libxsd-dev \
    xsd \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Создание рабочей директории
WORKDIR /build

# Копирование схемы XSD (нужна для генерации кода на этапе CMake)
COPY schema.xsd /build/schema.xsd

# Копирование исходных кодов back-end (логика взлома)
COPY back/ /build/back/

# Копирование исходных кодов front-end (HTTP сервер)
COPY front/ /build/front/

# Создание директории сборки
RUN mkdir -p /build/front/build

# Настройка и сборка проекта
WORKDIR /build/front/build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake/Qt6 \
    && cmake --build . -j$(nproc)

# =============================================================================
# ЭТАП ЗАПУСКА (RUNTIME)
# =============================================================================
FROM ubuntu:24.04 AS runtime

# Установка переменных окружения
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Moscow

# Установка только runtime зависимостей (без dev пакетов)
RUN apt-get update && apt-get install -y --no-install-recommends \
    libqt6core6 \
    libqt6network6 \
    libqt6httpserver6 \
    libqt6xml6 \
    libssl3 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/* \
    && apt-get clean

# Создание пользователя для запуска приложения (без root)
RUN useradd -m -s /bin/bash worker

# Создание рабочей директории
WORKDIR /app

# Копирование скомпилированного бинарника из builder стадии
COPY --from=builder /build/front/build/CrackHashWorker /app/CrackHashWorker

# Копирование схемы (может понадобиться для валидации или документации)
COPY --from=builder /build/schema.xsd /app/schema.xsd

# Установка правильных прав доступа
RUN chown -R worker:worker /app

# Переключение на пользователя worker
USER worker

# Проброс порта HTTP сервера
EXPOSE 8080

# Health check для проверки работоспособности сервиса
HEALTHCHECK --interval=30s --timeout=5s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/crack -X POST -H "Content-Type: application/xml" -d '<test/>' || exit 1

# Запуск приложения
ENTRYPOINT ["/app/CrackHashWorker"]
