# Makefile for compiling and linking .C files with asm files

CFILE = bench
ASMFILE = asm_partition_unroll
APPNAME = quicksort
CFLAGS = -masm=att -O2

DOCKER_COMPOSE=.docker/docker-compose.yml
DOCKERFILE=.docker/Dockerfile

PROF_OUT = profiling/$(APPNAME)_flat.prof
GPROF_FLAGS = -b -p

all: $(APPNAME)

$(CFILE): $(CFILE).c Makefile
	gcc $(CFLAGS) -S -o $@.s $<  

$(APPNAME): $(ASMFILE).S $(CFILE)
	gcc $(CFLAGS) -E $< -o $@_pp.s
	gcc $(CFLAGS) $(CFILE).s $@_pp.s -o $@

.PHONY: docker
docker: $(DOCKERFILE) $(DOCKER_COMPOSE)
	docker compose -f $(DOCKER_COMPOSE) build
	docker compose -f $(DOCKER_COMPOSE) up -d
	docker exec -it assembly_x86_64 bash ; docker compose -f $(DOCKER_COMPOSE) down

.PHONY: docker_fix
docker_fix:
	rm  ~/.docker/config.json 

.PHONY: prof
prof:
	gprof $(APPNAME) $(GPROF_FLAGS) >> $(PROF_OUT)

.PHONY: clean
clean:
	rm -rf *.dSYM *.o *.out *.s $(APPNAME)
