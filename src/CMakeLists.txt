add_library(xs SHARED
    xs.c
    ctx.c
    err.c
    instream.c
    msg.c
    mutex.c
    sock.c
    outstream.c
    wire.c
    plugin.c
    stdplugins.c
    threadpool.c
    transport_plugin.c
    inproc_transport.c
    pattern_plugin.c
    pair_pattern.c
    stream.c
    inproc_stream.c
    )
target_link_libraries(xs ${CMAKE_THREAD_LIBS_INIT})
install(TARGETS xs DESTINATION lib)
