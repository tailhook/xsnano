add_library(xs SHARED
    xs.c
    ctx.c
    err.c
    instream.c
    msg.c
    mutex.c
    pub.c
    sock.c
    sub.c
    outstream.c
    wire.c
    xpub.c
    xsub.c
    )
target_link_libraries(xs ${CMAKE_THREAD_LIBS_INIT})
install(TARGETS xs DESTINATION lib)
