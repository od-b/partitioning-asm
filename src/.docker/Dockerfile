FROM ubuntu:latest

WORKDIR /src

RUN apt update && apt upgrade -y && apt install -y build-essential

COPY . .

ENTRYPOINT ["tail", "-f", "/dev/null"]