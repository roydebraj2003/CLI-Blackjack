FROM ubuntu:rolling
WORKDIR /app
COPY . .
RUN echo "Updating ..."
RUN apt update && apt install -y gcc
RUN apt install libpq-dev -y && apt install libsodium-dev -y
RUN gcc main.c db.c -o Blackjack -I/usr/include/postgresql -lpq -lsodium
RUN echo "To connect to a database please provide your local DB_URL ..."
CMD ["./Blackjack"]

