# syntax=docker/dockerfile:1

FROM ubuntu:24.04 AS build
RUN apt-get update && apt-get install -y --no-install-recommends build-essential && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY . /app
RUN make

FROM ubuntu:24.04
WORKDIR /app
COPY --from=build /app/bin /app/bin
ENV PATH="/app/bin:${PATH}"
ENTRYPOINT ["/app/bin/interpretador"]
