# (C) Laurent Vivier <laurent@vivier.eu>

# Container support

CONTAINER_ENGINE ?= podman
ROOTFS_IMG ?= localhost/m68k-sid-rootfs:latest
EMILE_IMAGE ?= emile-m68k

.PHONY: import-rootfs
import-rootfs: .container/rootfs.tar.xz
	@if $(CONTAINER_ENGINE) image exists "$(ROOTFS_IMG)"; then \
		echo "$(ROOTFS_IMG) already present; skipping import"; \
	else \
		echo "Importing $(ROOTFS_IMG)"; \
		$(CONTAINER_ENGINE) import .container/rootfs.tar.xz $(ROOTFS_IMG); \
	fi

.PHONY: emile-m68k-container
emile-m68k-container: import-rootfs
	$(CONTAINER_ENGINE) build -f .container/Containerfile.m68k -t $(EMILE_IMAGE) --build-arg UID=$$(id -u) --build-arg GID=$$(id -g)
