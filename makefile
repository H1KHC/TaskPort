.PHONY: all clean directories distclean task_debug task_example task_release

all: task_debug task_example task_release

# Tasks:
task_debug: directories bin/libopentpd.a
	@echo "Task debug has been finished!"

task_example: directories bin/helloworld
	@echo "Task example has been finished!"

task_release: directories bin/libopentp.a
	@echo "Task release has been finished!"

# Targets:
bin/helloworld: bin/libopentpd.a .build/example/helloworld.cpp.helloworld.o
	@g++ -o $@ $^ -g -Lbin -lopentpd -pthread
	@echo "[ 100% ] Target helloworld has been built!"

bin/libopentp.a: .build/src/getCPUCoreCount.cpp.libopentp.a.o .build/src/openTP.cpp.libopentp.a.o .build/src/task/taskManager.cpp.libopentp.a.o .build/src/worker/worker.cpp.libopentp.a.o .build/src/worker/workerManager.cpp.libopentp.a.o
	@ar r $@ $^ 
	@echo "[ 100% ] Target libopentp.a has been built!"

bin/libopentpd.a: .build/src/getCPUCoreCount.cpp.libopentpd.a.o .build/src/openTP.cpp.libopentpd.a.o .build/src/task/taskManager.cpp.libopentpd.a.o .build/src/worker/worker.cpp.libopentpd.a.o .build/src/worker/workerManager.cpp.libopentpd.a.o
	@ar r $@ $^ 
	@echo "[ 100% ] Target libopentpd.a has been built!"

# Sources:
.build/example/helloworld.cpp.helloworld.o: example/helloworld.cpp include/openTP.h src/debug.h
	@echo "Generating sources for target helloworld..."
	@echo "[   0% ] Compiling .build/example/helloworld.cpp.helloworld.o..."
	@g++ -c -o $@ $< -g -DTP_STATIC -std=gnu++11 -Iinclude/ -Isrc/

.build/src/getCPUCoreCount.cpp.libopentp.a.o: src/getCPUCoreCount.cpp
	@echo "Generating sources for target libopentp.a..."
	@echo "[   0% ] Compiling .build/src/getCPUCoreCount.cpp.libopentp.a.o..."
	@g++ -c -o $@ $< -O2 -DTP_IMPLEMENT -DTP_STATIC -std=gnu++11 -Iinclude/ -Isrc/

.build/src/openTP.cpp.libopentp.a.o: src/openTP.cpp src/task/taskManager.h src/debug.h src/task/task.h include/openTP.h src/worker/workerManager.h src/worker/worker.h src/task/taskManager.h
	@echo "[  20% ] Compiling .build/src/openTP.cpp.libopentp.a.o..."
	@g++ -c -o $@ $< -O2 -DTP_IMPLEMENT -DTP_STATIC -std=gnu++11 -Iinclude/ -Isrc/

.build/src/task/taskManager.cpp.libopentp.a.o: src/task/taskManager.cpp src/debug.h src/worker/worker.h src/task/taskManager.h src/task/task.h include/openTP.h
	@echo "[  40% ] Compiling .build/src/task/taskManager.cpp.libopentp.a.o..."
	@g++ -c -o $@ $< -O2 -DTP_IMPLEMENT -DTP_STATIC -std=gnu++11 -Iinclude/ -Isrc/

.build/src/worker/worker.cpp.libopentp.a.o: src/worker/worker.cpp src/debug.h src/worker/worker.h src/worker/workerManager.h src/task/taskManager.h src/task/task.h include/openTP.h
	@echo "[  60% ] Compiling .build/src/worker/worker.cpp.libopentp.a.o..."
	@g++ -c -o $@ $< -O2 -DTP_IMPLEMENT -DTP_STATIC -std=gnu++11 -Iinclude/ -Isrc/

.build/src/worker/workerManager.cpp.libopentp.a.o: src/worker/workerManager.cpp src/debug.h src/worker/worker.h src/worker/workerManager.h src/task/taskManager.h src/task/task.h include/openTP.h
	@echo "[  80% ] Compiling .build/src/worker/workerManager.cpp.libopentp.a.o..."
	@g++ -c -o $@ $< -O2 -DTP_IMPLEMENT -DTP_STATIC -std=gnu++11 -Iinclude/ -Isrc/

.build/src/getCPUCoreCount.cpp.libopentpd.a.o: src/getCPUCoreCount.cpp
	@echo "Generating sources for target libopentpd.a..."
	@echo "[   0% ] Compiling .build/src/getCPUCoreCount.cpp.libopentpd.a.o..."
	@g++ -c -o $@ $< -g -D_DEBUG -DTP_IMPLEMENT -DTP_STATIC -std=gnu++11 -Iinclude/ -Isrc/

.build/src/openTP.cpp.libopentpd.a.o: src/openTP.cpp src/task/taskManager.h src/debug.h src/task/task.h include/openTP.h src/worker/workerManager.h src/worker/worker.h src/task/taskManager.h
	@echo "[  20% ] Compiling .build/src/openTP.cpp.libopentpd.a.o..."
	@g++ -c -o $@ $< -g -D_DEBUG -DTP_IMPLEMENT -DTP_STATIC -std=gnu++11 -Iinclude/ -Isrc/

.build/src/task/taskManager.cpp.libopentpd.a.o: src/task/taskManager.cpp src/debug.h src/worker/worker.h src/task/taskManager.h src/task/task.h include/openTP.h
	@echo "[  40% ] Compiling .build/src/task/taskManager.cpp.libopentpd.a.o..."
	@g++ -c -o $@ $< -g -D_DEBUG -DTP_IMPLEMENT -DTP_STATIC -std=gnu++11 -Iinclude/ -Isrc/

.build/src/worker/worker.cpp.libopentpd.a.o: src/worker/worker.cpp src/debug.h src/worker/worker.h src/worker/workerManager.h src/task/taskManager.h src/task/task.h include/openTP.h
	@echo "[  60% ] Compiling .build/src/worker/worker.cpp.libopentpd.a.o..."
	@g++ -c -o $@ $< -g -D_DEBUG -DTP_IMPLEMENT -DTP_STATIC -std=gnu++11 -Iinclude/ -Isrc/

.build/src/worker/workerManager.cpp.libopentpd.a.o: src/worker/workerManager.cpp src/debug.h src/worker/worker.h src/worker/workerManager.h src/task/taskManager.h src/task/task.h include/openTP.h
	@echo "[  80% ] Compiling .build/src/worker/workerManager.cpp.libopentpd.a.o..."
	@g++ -c -o $@ $< -g -D_DEBUG -DTP_IMPLEMENT -DTP_STATIC -std=gnu++11 -Iinclude/ -Isrc/

# Others:
directories:
	@mkdir -p .build/ .build/example .build/src .build/src/task .build/src/worker bin/
	@echo "Directories has been created!"

distclean:
	@rm -rf .build/
	@echo "Cleaned!"

clean:
	@rm -rf .build/ .build/example .build/src .build/src/task .build/src/worker bin/ bin/helloworld bin/libopentp.a bin/libopentpd.a
	@echo "Cleaned!"
