FROM archlinux:latest

RUN pacman -Sy --noconfirm \
    qt6-base \
    qt6-httpserver \
    xerces-c \
    libx11 \
    libxcb \
    && pacman -Scc --noconfirm

WORKDIR /app
COPY ./deploy/bin/CrackManager /app/app

RUN chmod +x /app/app

EXPOSE 8080

ENTRYPOINT ["/app/app"]
