### Build/test container ###
FROM iceberg-webserver:base as builder

COPY . /usr/src/projects/iceberg-webserver/
WORKDIR /usr/src/projects/iceberg-webserver/build

RUN cmake ..
RUN make
RUN ctest --output-on_failure


### Deploy container ###
FROM ubuntu:latest as deploy

# TODO(!): Copy server output binary to "."
# COPY --from=builder /usr/src/project/build/bin/example_server .
COPY --from=builder /usr/src/projects/iceberg-webserver/build/bin/server .
COPY --from=builder /usr/src/projects/iceberg-webserver/build/config ./config
COPY --from=builder /usr/src/projects/iceberg-webserver/file1 /usr/src/projects/iceberg-webserver/file1
COPY --from=builder /usr/src/projects/iceberg-webserver/file2 /usr/src/projects/iceberg-webserver/file2
COPY --from=builder /usr/src/projects/iceberg-webserver/file2 /usr/src/projects/iceberg-webserver/file2
COPY --from=builder /usr/src/projects/iceberg-webserver/tests/static_file /usr/src/projects/iceberg-webserver/tests/static_file

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
