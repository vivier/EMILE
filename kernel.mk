#
# (c) 2004-2013 Laurent Vivier <Laurent@Vivier.EU>
#

# Kernel architecture

LINUX=$(shell ls $(LINUXPATH) 2> /dev/null)

ifeq ($(LINUX),$(LINUXPATH))
        FILEARCH=$(shell file -bknL $(LINUX) | cut -d, -f 2)
        ifeq ($(findstring PowerPC, $(FILEARCH)), PowerPC)

                KARCH=ppc
                KSTRIP=$(PPC_STRIP)

        else
        ifeq ($(findstring Motorola 68000, $(FILEARCH)), Motorola 68000)

                KARCH=classic
                KSTRIP=$(M68K_STRIP)

        else
        ifeq ($(findstring Motorola 68, $(FILEARCH)), Motorola 68)

                KARCH=m68k
                KSTRIP=$(M68K_STRIP)
        else
                KARCH=unknown
                KSTRIP=$(M68K_STRIP)
        endif
        endif
        endif
else
        KARCH=m68k
        KSTRIP=$(M68K_STRIP)
endif
