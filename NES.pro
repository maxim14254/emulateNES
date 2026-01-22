TEMPLATE = subdirs

BIN_DIR = $$shadowed(emulateNES)
LOG = 0
DEBUG = 1

SUBDIRS += \
    emulateNES \


equals(LOG, 1){

    DEFINES += LOG_ON

    SUBDIRS += testNES \

    QMAKE_POST_LINK += $$QMAKE_COPY "$$PWD/testNES/benchmark_log.txt" "$$BIN_DIR/benchmark_log.txt"
}

equals(DEBUG, 1){

    DEFINES += DEBUG_ON
}

