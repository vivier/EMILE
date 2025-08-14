# (C) Laurent Vivier <laurent@vivier.eu>

# Container support

CONTAINER_ENGINE ?= podman

TAG_SID=20210211
TAR_SID=rootfs-sid-$(TAG_SID).tar.xz
ROOTFS_SID_IMG ?= m68k/sid/rootfs:$(TAG_SID)

TAG_ETCH=latest
TAR_ETCH=rootfs-etch-$(TAG_ETCH).tar.xz
ROOTFS_ETCH_IMG ?= m68k/etch/rootfs:$(TAG_ETCH)

EMILE_IMAGE_SID ?= emile-m68k-sid
EMILE_IMAGE_ETCH ?= emile-m68k-etch

.PHONY: import-sid-rootfs
import-sid-rootfs: .container/$(TAR_SID)
	@if $(CONTAINER_ENGINE) image exists "$(ROOTFS_SID_IMG)"; then \
		echo "$(ROOTFS_SID_IMG) already present; skipping import"; \
	else \
		echo "Importing $(ROOTFS_SID_IMG)"; \
		$(CONTAINER_ENGINE) import --arch m68k --os debian --variant sid $< "$(ROOTFS_SID_IMG)"; \
	fi

.PHONY: import-etch-rootfs
import-etch-rootfs: .container/$(TAR_ETCH)
	@if $(CONTAINER_ENGINE) image exists "$(ROOTFS_ETCH_IMG)"; then \
		echo "$(ROOTFS_ETCH_IMG) already present; skipping import"; \
	else \
		echo "Importing $(ROOTFS_ETCH_IMG)"; \
		$(CONTAINER_ENGINE) import --arch m68k --os debian --variant etch $< "$(ROOTFS_ETCH_IMG)"; \
	fi


.PHONY: emile-m68k-sid-container
emile-m68k-sid-container: import-sid-rootfs
	$(CONTAINER_ENGINE) build --pull=never -f .container/Containerfile-sid.m68k -t $(EMILE_IMAGE_SID) \
				  --build-arg UID=$$(id -u) \
				  --build-arg GID=$$(id -g)
.PHONY: emile-m68k-etch-container
emile-m68k-etch-container: import-etch-rootfs
	$(CONTAINER_ENGINE) build --pull=never -f .container/Containerfile-etch.m68k -t $(EMILE_IMAGE_ETCH) \
				  --build-arg UID=$$(id -u) \
				  --build-arg GID=$$(id -g)

DATE=$(shell date +%Y%m%d)
.PHONY:
emile-containers-export:
	-mkdir archives
	$(CONTAINER_ENGINE) create --name etch-container-temp localhost/$(EMILE_IMAGE_ETCH)
	$(CONTAINER_ENGINE) export -o archives/$(EMILE_IMAGE_ETCH)-$(DATE).tar etch-container-temp
	$(CONTAINER_ENGINE) rm etch-container-temp
	xz -9 archives/$(EMILE_IMAGE_ETCH)-$(DATE).tar
	$(CONTAINER_ENGINE) create --name sid-container-temp localhost/$(EMILE_IMAGE_SID)
	$(CONTAINER_ENGINE) export -o archives/$(EMILE_IMAGE_SID)-$(DATE).tar sid-container-temp
	$(CONTAINER_ENGINE) rm sid-container-temp
	xz -9 archives/$(EMILE_IMAGE_SID)-$(DATE).tar
