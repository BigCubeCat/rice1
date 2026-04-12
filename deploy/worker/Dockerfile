FROM archlinux:latest

RUN pacman -Sy --noconfirm \
    qt6-base \
    qt6-httpserver \
    xerces-c \
    libx11 \
    libxcb \
    && pacman -Scc --noconfirm

ENV QT_LOGGING_RULES="*=true"
ENV QT_FORCE_STDERR_LOGGING=1

WORKDIR /app
COPY ./deploy/bin/CrackHashWorker /app/app

RUN chmod +x /app/app

EXPOSE 8080

ENTRYPOINT ["/app/app"]
