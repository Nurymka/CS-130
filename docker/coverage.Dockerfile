### Build/test container ###
FROM iceberg-webserver:base as builder

COPY . /usr/src/project
WORKDIR /usr/src/project/build

RUN mkdir build_coverage
RUN cd build_coverage
RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN make coverage


### Deploy container ###
FROM ubuntu:latest as deploy

# TODO(!): Copy server output binary to "."
# COPY --from=builder /usr/src/project/build/bin/example_server .
COPY --from=builder /usr/src/project/build/bin/server .
COPY --from=builder /usr/src/project/build/config ./config

# TODO(!): Expose some port(s)
# EXPOSE 80
EXPOSE 80

# Use ENTRYPOINT to specify the binary name
# TODO(!): Update with real server name
# ENTRYPOINT ["./example_server"]
ENTRYPOINT [ "./server" ]

# Use CMD to specify arguments to ENTRYPOINT
# TODO(!): Update with real server args
CMD ["./config/server_config"]
