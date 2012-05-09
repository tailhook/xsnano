add_library(xs SHARED
    xs.c
    ctx.c
    err.c
    msg.c
    mutex.c
    pub.c
    sock.c
    sub.c
    tcpout.c
    xpub.c
    xsub.c
    )
target_link_libraries(xs ${CMAKE_THREAD_LIBS_INIT})
install(TARGETS xs DESTINATION lib)
